/****************************************************************************************
*	Програмный модуль для переноса результатов рассчетов из временного файла            *
*   calc_resalt_temp.txt в постоянный пополняемый и расширяемый файл                    *
*   q_tree.txt и корректировки файла task_globe.txt                                                                    *
*****************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char **argv)
{
    int n_point;        /*ключевой номер точки*/
    double p1;          /*первая координата*/
    double p2;          /*вторая координата*/
    double f1;          /*первое значение*/
    double f2;          /*второе значение*/
    double f3;          /*третье значение*/

    // вводим переменные для подсчета посчитанных точек,
    // из них не нулевых
    // и определения максимального значения

    int n_point_plus = 0;
    int n_point_plus_no_zero = 0;
    double f1_max_local =0;

    // читаем текущее минимальное значение функции распределения

    FILE *task_q;

    char *filename1 = argv[1];
        if((task_q = fopen(filename1,"r")) == NULL)
        {
        printf("Error opening file task_q.txt\n");
        return 1;
        }

    char s[100];
    int int_term;
    double double_term;
    double f1_min;  
    
    
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%d %s", &int_term, s);    
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &f1_min, s);
    fscanf(task_q, "%lf %s", &double_term, s);

    fclose(task_q);

    // printf("f1_min = %d\n", f1_min);

    int n_point_t; 
    char first_child_t[17];
    char parent_t[17];
    int g_number_t;
    char point_id_t[17];
    double p1_t;
    double p2_t;
    char calculated_t[2];
    double f1_t;
    double f2_t;
    double f3_t;

  FILE *calc_resalt_temp;

    if((calc_resalt_temp = fopen("/home/gzimin/Documents/University/Fen_2020_Zim/calc_result","r")) == NULL)

        {
            printf("Error opening file calc_resalt_temp.txt\n");
            return 1;
        };
    
    while (fscanf (calc_resalt_temp, "%d %lf %lf %lf %lf %lf", &n_point, &p1, &p2, &f1, &f2, &f3) != EOF)
      {
        n_point_plus = n_point_plus + 1;
        // printf("n_point_plus = %d\n", n_point_plus);

        if(f1 > f1_min)
            {
                n_point_plus_no_zero = n_point_plus_no_zero + 1;
                // printf("n_point_plus_no_zero = %d\n", n_point_plus_no_zero);
            }

        if(f1 > f1_max_local)
            {
                f1_max_local = f1;
                // printf("f1_max_local = %d\n", f1_max_local);
            }

        FILE *q_tree;

        char *filename3 = argv[3];
        if((q_tree = fopen("q_tree.txt","r")) == NULL)
        {
        printf("Error opening file q_tree.txt\n");
        return 1;
        }

        FILE *q_tree_n;

        if((q_tree_n = fopen("q_tree_n.txt","a")) == NULL)
        {
        printf("Error creation file q_tree_n.txt\n");
        return 1;
        }

       while (fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t, first_child_t, parent_t, &g_number_t, point_id_t, &p1_t, &p2_t, calculated_t, &f1_t, &f2_t, &f3_t) != EOF)
       {
             // printf ("n_point_t = %d\n", n_point_t);
              if (n_point_t == n_point)
              {
                // printf ("n_point = n_point_t = %d %.10e %.10e %.10e\n", n_point, f1, f2, f3);
                fprintf (q_tree_n, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point, first_child_t, parent_t, g_number_t, point_id_t, p1_t, p2_t, "1", f1, f2, f3);
              }
              else
              {
                //printf ("n_point = n_point_t = %d %.10e %.10e %.10e\n", n_point, f1, f2, f3);
                fprintf (q_tree_n, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t, first_child_t, parent_t, g_number_t, point_id_t, p1_t, p2_t, calculated_t, f1_t, f2_t, f3_t);
              };
        };
        fclose(q_tree);
        fclose(q_tree_n);
        remove("Fen_2020_Zim//q_tree.txt");
        rename("Fen_2020_Zim//q_tree_n.txt", "Fen_2020_Zim//q_tree.txt");
      };     

    fclose(calc_resalt_temp);
    remove("Fen_2020_Zim//calc_resalt_temp.txt");

    // читаем и редактируем глобальные параметры в файле task_globe

    int n_point_calc;
    int n_point_no_zero;
    double f1_max;
    int temp1;
    int temp2;

    FILE *task_globe;

        char *filename2 = argv[2];

        if((task_globe = fopen(filename2,"r")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }

        fscanf (task_globe, "%d %d %le %d %d", &n_point_calc, &n_point_no_zero, &f1_max, &temp1, &temp2);

        fclose(task_globe);

        if((task_globe = fopen(filename2,"w")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }

        if(f1_max_local > f1_max)
            {
                f1_max =  f1_max_local;
            }


        fprintf (task_globe, "%d %d %e %d %d \n", n_point_calc + n_point_plus, n_point_no_zero + n_point_plus_no_zero, f1_max, temp1, temp2);

        fclose(task_globe);
    
    return 0;
}