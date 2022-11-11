// author: R10521802

#include <iostream>
#include <vector>
using namespace std;

const int ZONES = 24;

void readInput(int** shifts, int** workerDemands, int** leaveDemands, int nK, int nJ, int R) {
    // read shifts
    for (int k = 1; k <= nK; k++) {
        shifts[k] = new int[ZONES];
        for (int z = 0; z < ZONES; z++) {
            cin >> shifts[k][z];
        }
    }
    shifts[0] = new int[ZONES];
    for (int z = 0; z < ZONES; z++) {
        cin >> shifts[0][z];
    }
    // read workerDemands
    for (int j = 0; j < nJ; j++) {
        workerDemands[j] = new int[ZONES];
        for (int z = 0; z < ZONES; z++) {
            cin >> workerDemands[j][z];
        }
    }
    // read leave workerDemands
    // 1st row: worker id
    // 2nd row: leave demand of day
    for (int i = 0; i < 2; i++) {
        leaveDemands[i] = new int[R];
        for (int r = 0; r < R; r++) {
            cin >> leaveDemands[i][r];
        }
    }
}

int CalculateBestShiftDemandAndUpdateNaive(int** shifts, int** workerDemands, int* shiftDemand, int nJ, int nK) {
    int* shiftWorkerDemand = new int[nK+1];
    for (int i = 0; i <= nK; i++) shiftWorkerDemand[i] = 0;
    
    for (int j = 0; j < nJ; j++) {
        for (int k = 0; k <= nK; k++) {
            for (int z = 0; z < ZONES; z++) {
                if (shifts[k][z] == 1) {
                    shiftWorkerDemand[k] += workerDemands[j][z];
                }
            }
        }
        int sum = 0;
        for (int z = 0; z < ZONES; z++) {
            sum += workerDemands[j][z];
        }
        shiftDemand[j] = sum / ZONES;  

    }

    int maxIndex = 0;
    for (int k = 0; k <= nK; k++) {
        if (shiftWorkerDemand[k] > shiftWorkerDemand[maxIndex]) {
            maxIndex = k;
        }
    }
    // release memory
    delete[] shiftWorkerDemand;

    return maxIndex;
}

void arrangeWorkSchedule(int** workSchedule, int** shifts, int** workerDemands, int** leaveDemands, int nI, int nJ, int nK, int L, int R) {
    // initialize workSchedule to -1
    for (int i = 0; i < nI; i++) {
        workSchedule[i] = new int[nJ];
        for (int j = 0; j < nJ; j++) {
            workSchedule[i][j] = -1;
        }
    }

    // Average demand of each day
    int* shiftDemand = new int[nJ];
    for (int j = 0; j < nJ; j++) shiftDemand[j] = 0;
    int bestShiftIndex = CalculateBestShiftDemandAndUpdateNaive(shifts, workerDemands, shiftDemand, nJ, nK);

    // workdays of each worker
    int* workdays = new int[nI];
    for (int i = 0; i < nJ; i++) workdays[i] = 0;

    for (int j = 0; j < nJ; j++) {
        int demand = shiftDemand[j];
        for (int i = 0; i < nI; i++) {
            if (demand > 0) {
                // if the worker has no workdays left, day off
                if (workdays[i] == nJ - L) {
                    workSchedule[i][j] = 0;
                    continue;
                }
                // if the worker has 6 consecutive workdays, day off
                if (j >= 6 && workSchedule[i][j-1] != 0) {
                    int consecutiveWorkdays = 1;
                    for (int k = j-2; k >= 0; k--) {
                        if (workSchedule[i][k] != 0) {
                            consecutiveWorkdays++;
                        } else {
                            break;
                        }
                    }
                    if (consecutiveWorkdays >= 6) {
                        workSchedule[i][j] = 0;
                        continue;
                    }
                }
                // if the worker hasn't been assigned the shift
                if (workSchedule[i][j] == -1) {
                    // check if the worker has a leave demand on that day
                    bool hasLeaveDemand = false;
                    for (int r = 0; r < R; r++) {
                        if (leaveDemands[0][r] == i+1 && leaveDemands[1][r] == j+1) {
                            hasLeaveDemand = true;
                            break;
                        }
                    }
                    if (hasLeaveDemand) {
                        workSchedule[i][j] = 0;
                    }
                    else {
                        workSchedule[i][j] = bestShiftIndex;
                        demand--;
                        workdays[i]++;
                    }
                    // // TA-naiive
                    // workSchedule[i][j] = bestShiftIndex;
                    // demand--;
                    // workdays[i]++;
                }
            }
            else {
                workSchedule[i][j] = 0;
            }
        }
    }
    // release memory
    delete[] shiftDemand;
    delete[] workdays;
}


int main() {
    int nI, nJ, nK, L, w1, w2, R;
    cin >> nI >> nJ >> nK >> L >> w1 >> w2 >> R;
    int** shifts = new int*[nK+1];
    int** workerDemands = new int*[nJ];
    int** leaveDemands = new int*[2];
    readInput(shifts, workerDemands, leaveDemands, nK, nJ, R);
    
    // workSchedule[i][j] means the shift of worker i on day j
    int** workSchedule = new int*[nI];
    arrangeWorkSchedule(workSchedule, shifts, workerDemands, leaveDemands, nI, nJ, nK, L, R);

    // print workSchedule
    for (int i = 0; i < nI; i++) {
        for (int j = 0; j < nJ; j++) {
            cout << workSchedule[i][j];
            (j == nJ-1) ? cout << " " : cout << ",";
        }
        cout << endl;
    }

    // // calculate the lack of workers
    // int lack = 0;
    // for (int day = 0; day < nJ; day++) {
    //     int workersPerDay[ZONES] = {0};
    //     for (int worker = 0; worker < nI; worker++) {
    //         for (int h = 0; h < ZONES; h++) 
    //             workersPerDay[h] += shifts[workSchedule[worker][day]][h];
    //     }
    //     for (int h = 0; h < ZONES; h++) 
    //         lack += max(0, workerDemands[day][h] - workersPerDay[h]);
    // }
    // cout << "lack: " << lack << endl;

    // release memory
    for (int i = 0; i < nK+1; i++) delete[] shifts[i];
    for (int i = 0; i < nJ; i++) delete[] workerDemands[i];
    for (int i = 0; i < 2; i++) delete[] leaveDemands[i];
    for (int i = 0; i < nI; i++) delete[] workSchedule[i];
    delete[] shifts;
    delete[] workerDemands;
    delete[] leaveDemands;
    delete[] workSchedule;
    return 0;
}