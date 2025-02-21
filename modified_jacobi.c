#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

int main()
{
    float a[10][10], b[10], x[10], xn[10], sum, e;
    int i, j, n, flag = 0, key;
    printf("\nThis program illustrates Gauss-Jacobi method to solve system of AX=B\n");
    printf("\nEnter the dimensions of coefficient matrix n: ");
    scanf("%d", &n)    ;
    printf("\nEnter the elements of matrix A:\n");
    
    //1.1 Free From Loop Carried Dep = No loop carried dependency as we are reading values of different independent equations 
    for (i = 0; i < n; i++)
    {
        //1.2 Free From Loop Carried Dep: As independent values of coefficient being read
        for (j = 0; j < n; j++)
        {
            scanf("%f", &a[i][j]);
        }
    }
    printf("\nEnter the elements of matrix B:\n");
    
    //1.3 Free From Loop Carried Dep: As independent values of coefficient for b being read
    for (i = 0; i < n; i++)
        scanf("%f", &b[i]);
    printf("\nThe system of linear equations:\n");

    //1.4 Free from Loop carried dependency as printing different linear equation
    for (i = 0; i < n; i++)
    {
        printf("\n(%.2f)x1+(%.2f)x2+(%.2f)x3=(%.2f)\n", a[i][0], a[i][1], a[i][2], b[i]);
    }

    //1.5 Free from Loop carried dependency as we are checking each row individually
    //OpenMP does not provide a mechanism to break out of a parallel loop.
    //So no paralleisation without adding additional overhead
    for (i = 0; i < n; i++)
    {
        sum = 0;
        // Debatable Loop Carried dependency because since sum used in prev iteration is being used in the next but this loop can be parallelized
        for (j = 0; j < n; j++)
        {
            sum += fabs(a[i][j]);
        }
        sum -= fabs(a[i][i]);
        if (fabs(a[i][i] < sum))
        {
            flag = 1;
            break;
        }
    }

    if (flag == 1)
    {
        printf("\nThe system of linear equations are not diagonally dominant\n");
        return main();
    }
    else
    {
        printf("\nThe system of linear equations are diagonally dominant\n");
        printf("\nEnter the initial approximations: ");
        for (i = 0; i < n; i++)
        {
            printf("\nx%d=", (i + 1));
            scanf("%f", &x[i]);
        }
        printf("\nEnter the error tolerance level:\n");
        scanf("%f", &e);
    }

    printf("x[1]\t\tx[2]\t\tx[3]");
    printf("\n");
    key = 0;

    printf("\n\nPrinting with Dynamic and static Scheduling\n");
    
    clock_t start_time = clock();
    while (key < n - 1)
    {
        key = 0;

        //1.6 free from loop carrieed dependecny because in vlaues supplied of xn are within the scope of the current while loop iteration

        // Parallelizing with guided because differnet iteration(chunks) might have to different amt of work due to internal if statements
        //Generally, guided scheduling is faster than dynamic scheduling because it adapts chunk sizes to balance workload distribution and reduce scheduling overhead, particularly for loops with irregular workloads or unpredictable iteration execution times13. 
        //Starting out with larger chgunk sizes and then adapting.However, the performance difference can vary depending on the specific application characteristics 
        #pragma omp parallel for private(sum, j) shared(a, b, x, xn, key) schedule(dynamic)
        for (i = 0; i < n; i++)
        {
            sum = b[i];
            for (j = 0; j < n; j++)
                if (j != i)
                    sum -= a[i][j] * x[j];
            xn[i] = sum / a[i][i];
            if (fabs(x[i] - xn[i]) < e)
            {
                key = key + 1;
            }
        }
        if (key == n)
        {
            break;
        }
        printf("%f\t %f\t %f\t", xn[0], xn[1], xn[2]);

        // All threads do the same work, hence static schedulign fixed and equal chunks
        #pragma omp parallel for shared(x, xn) schedule(static)
        for (i = 0; i < n; i++)
        {
            x[i] = xn[i];
        }
    }
    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("\n\nTime taken: %f seconds\n", time_taken);
    
    
    printf("\nAn approximate solution to the given system of equations is\n");
    for (i = 0; i < n; i++)
    {
        printf("\nx[%d]=%f\n", (i + 1), x[i]);
    }
    return 0;
}
