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

void CalculateBestShiftDemandAndUpdateNaive(int** shifts, int** workerDemands, int* shiftWorkerDemand, int nJ, int nK, int day, int* bestShift, int* ifnight) { // 找出最好的班型
    for (int i = 0; i <= nK; i++) shiftWorkerDemand[i] = 0;
    for (int k = 0; k <= nK; k++) {   //shift
        for (int z = 0; z < ZONES; z++) {  //時段
            if (shifts[k][z] == 1 && workerDemands[day][z] > 0) {
                shiftWorkerDemand[k]++;
            }
        }
    }
    
    // 分早晚班
    int maxIndex = 0, maxIndex_night = 0, maxIndex_nonNight = 0;
    for (int k = 0; k <= nK; k++)
    {

        if(ifnight[k] == 0) // 非晚班
        {
            if (shiftWorkerDemand[k] > shiftWorkerDemand[maxIndex_nonNight])
                maxIndex_nonNight = k;
        }
        else {
            if (shiftWorkerDemand[k] > shiftWorkerDemand[maxIndex]) {
                maxIndex = k;
            }

        }
    }

    bestShift[0] = maxIndex;
    bestShift[1] = maxIndex_nonNight;

    // cout << endl << "Day " << day << endl;
    // for (int k = 0; k <= nK; k++) {
    //     cout << shiftWorkerDemand[k] << " ";
    // }

    // cout << endl << "Best shift: " << bestShift[0] << " " << bestShift[1] << endl;
    // return maxIndex;
}


void arrangeWorkSchedule(int** workSchedule, int** shifts, int** workerDemands, int** leaveDemands, int nI, int nJ, int nK, int L, int R, int* bestShift, int w1, int w2) {
    // initialize workSchedule to -1
    for (int i = 0; i < nI; i++) { // 員工*天數
        workSchedule[i] = new int[nJ];
        for (int j = 0; j < nJ; j++) {
            workSchedule[i][j] = -1;
        }
    }
    // 夜班=1, 白班=0
    int* ifnight = new int[nK+1];
    for (int i = 0; i <= nK; i++) ifnight[i] = 0;
    for(int i = 0; i <= nK; i++) // 判斷當下班型是否含夜班
    {
        for(int j = 18; j <= ZONES; j++)
        {
            if(shifts[i][j] == 1)
            {
                ifnight[i] = 1;
                break;
            }
        }
    }
    // 當天每個班型的總需求人數
    int* shiftWorkerDemand = new int[nK+1];

    // workdays of each worker
    int* workdays = new int[nI];
    for (int i = 0; i < nI; i++) workdays[i] = 0; 
    
    for (int j = 0; j < nJ; j++) { // 天數

        for (int i = 0; i < nI; i++) {   // 員工數

            CalculateBestShiftDemandAndUpdateNaive(shifts, workerDemands, shiftWorkerDemand, nJ, nK, j, bestShift, ifnight); // 找出最好的排班
            if (shiftWorkerDemand[bestShift[0]] > 0 || shiftWorkerDemand[bestShift[1]] > 0) { // 有人可以排
                // if the worker has no workdays left, day off
                
                if (workdays[i] == nJ - L) {
                    // cout << "Worker " << i+1 << ", on day " << j+1 <<" has no workdays left" << endl;
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
                    // bool hasLeaveDemand = false;
                    // for (int r = 0; r < R; r++) {
                    //     if (leaveDemands[0][r] == i+1 && leaveDemands[1][r] == j+1) { // i員工在第j天要休假
                    //         hasLeaveDemand = true;
                    //         break;
                    //     }
                    // }
                    // if (hasLeaveDemand) {
                    //     workSchedule[i][j] = 0;
                    // }

                    // else {
                    //     workSchedule[i][j] = bestShift[0];
                    //     demand--; 
                    //     workdays[i]++;
                    // }
                    // TA-naiive
                    int overNightShift = 0;
                    workSchedule[i][j] = bestShift[0]; 
                    
                    if(ifnight[workSchedule[i][j]] == 1) // 如果目前的班型為夜班
                    {
                        if (j >= 1) 
                        {
                            int consecutiveWorkdays = 1;
                            for (int k = j-1; k >= 0; k--) //從前一天開始找，找到前6天
                            {
                                consecutiveWorkdays++;
                                if(consecutiveWorkdays > 6)
                                    break;
                                if(ifnight[workSchedule[i][k]] == 1)
                                {
                                    overNightShift = 1;
                                    break;
                                }
                            }
                        }
                        
                        if(overNightShift == 1) //如果前面有夜班，就用非夜班的班型
                            workSchedule[i][j] = bestShift[1];
                    }

                    if (workSchedule[i][j] != 0)
                    {
                        workdays[i]++;
                    }
    
                    for (int k = 0; k <= ZONES; k++) 
                        workerDemands[j][k] -= shifts[workSchedule[i][j]][k];  //該時段的需求-1

                }
            }
            else {
                workSchedule[i][j] = 0;
            }

        }
        // 印出workdays
        // cout << "workdays: " << j+1 << endl;
        // for (int i = 0; i < nI; i++) {
        //     cout << " " << workdays[i];
        // }
        // cout << endl;
    }
    
    // 缺工數 (第i天第z個時段)
    int** lackPerZone = new int*[nJ];
    for (int i = 0; i < nJ; i++) {
        lackPerZone[i] = new int[ZONES];
        for (int z = 0; z <= ZONES; z++) {
            lackPerZone[i][z] = 0;
        }
    }

    // 計算缺工數
    for (int day = 0; day < nJ; day++) {
        int workersPerDay[ZONES] = {0};
        for (int worker = 0; worker < nI; worker++) {
            for (int h = 0; h < ZONES; h++) 
                workersPerDay[h] += shifts[workSchedule[worker][day]][h];
        }
        for (int h = 0; h < ZONES; h++) 
            lackPerZone[day][h]= workerDemands[day][h] - workersPerDay[h];
    }

    
    for (int r = 0; r < R; r++) // i員工在第j天要休假
    {
        int counts = 0;
        int i = leaveDemands[0][r] - 1, j = leaveDemands[1][r] - 1; // i 員工編號 j 天數
        if (workSchedule[i][j] != 0) // 該天有上班，但要請假
        {
            for (int h = 0; h < ZONES; h++)
            {
                // 該員工在該時段要上班，請假使得該時段缺工
                if(lackPerZone[j][h] >= 0 && shifts[workSchedule[i][j]][h] == 1)
                    counts++;
                
            }
            // 請假後的目標函數較小，則請假
            if(w1 > counts) {
                workSchedule[i][j] = 0;
                workdays[i]--;
                // 更新每天每個時段的缺工數
                for (int h = 0; h < ZONES; h++)
                {
                    if(shifts[workSchedule[i][j]][h] == 1)
                        lackPerZone[j][h]++;
                }
            }
        }
    }

    
    // cout << bestShift[0] << "," << bestShift[1];
    // release memory
    delete[] shiftWorkerDemand;
    // delete[] workdays;
    for (int i = 0; i < nJ; i++) {
        delete[] lackPerZone[i];
    }
    delete[] lackPerZone;
}


int main() {
    int nI, nJ, nK, L, w1, w2, R;
    cin >> nI >> nJ >> nK >> L >> w1 >> w2 >> R;
    int** shifts = new int*[nK+1];
    int** workerDemands = new int*[nJ];
    int** leaveDemands = new int*[2];
    readInput(shifts, workerDemands, leaveDemands, nK, nJ, R);
    int bestShift[2] = {0};
    // workSchedule[i][j] means the shift of worker i on day j
    int** workSchedule = new int*[nI];
    
    arrangeWorkSchedule(workSchedule, shifts, workerDemands, leaveDemands, nI, nJ, nK, L, R, bestShift, w1, w2);

    // print workSchedule
    for (int i = 0; i < nI; i++) {
        for (int j = 0; j < nJ; j++) {
            cout << workSchedule[i][j];
            (j == nJ-1) ? cout << " " : cout << ",";
        }
        cout << endl;
    }

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