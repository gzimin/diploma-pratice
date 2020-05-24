/****************************************************************************************
*	Програмный модуль для проверки качества воспроизведения функции распределения       *
*   по итогам очередной итерации и добавление новых узлов (листьев) в квадродерево      *
*   при необходимости                                                                   *
*   если они есть, то, возможно, надо увеличивать область покрытия                      *
*   на вход даём файлы task_q.txt, task_globe.txt и q_tree.txt                                          *
*****************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int main (int argc, char **argv)
{
    /* Определяем функцию возведения в положительную целую степень для целых чисел          */

    int Pow_int(int int_number, int int_degree)
    {
        int Pow_resalt = 1;
        int i;
        for ( i = 0; i < int_degree; i++)
        {
        Pow_resalt = Pow_resalt * int_number;
        };
        return Pow_resalt;
    }

    // читаем из файла task_q.txt сторону (размер) квадродерева size_0
    // читаем из файла task_q.txt минимально допустимый шаг точек квадродерева size_3
    // читаем из файла task_q.txt допустимую относительную погрешность при линейной интерполяции allowable_error
    
    printf("files = %s %s %s\n", argv[1], argv[2], argv[3]);

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
    double size_0;
    double size_3;
    double f_min;
    double allowable_error;
    
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
    fscanf(task_q, "%lf %s", &size_0, s);
    fscanf(task_q, "%lf %s", &double_term, s);
    fscanf(task_q, "%lf %s", &size_3, s);
    fscanf(task_q, "%lf %s", &f_min, s);
    fscanf(task_q, "%lf %s", &allowable_error, s);
      
    fclose(task_q);

    // читаем из файла task_globe.txt текущее число поколений узлов квадродерева
    // читаем из файла task_globe.txt число точек, добавленных на предыдущей итерации
    // если оно равно нулю, то делать больше нечего

FILE *task_globe;

        char *filename2 = argv[2];
        if((task_globe = fopen(filename2,"r")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }

    int g_number_in;
    int n_point_last; 
    int int_temp_2;
    double double_term_2;

    fscanf (task_globe, "%d %d %lf %d %d", &int_temp_2, &int_temp_2, &double_term_2, &g_number_in, &n_point_last);

    if(n_point_last == 0)
        {
        printf("На предыдущей итерации точек не добавлялось!\n");
        return 0;
        }

    if(size_0 / Pow_int(2,g_number_in + 2) < size_3)
        {
        printf("На предыдущей итерации был достигнут установленный предел шага сетки!\n");
        return 0;
        }

     fclose(task_globe);

    FILE *q_tree;

        char *filename3 = argv[3];
        if((q_tree = fopen(filename3,"r")) == NULL)
        {
        printf("Error opening file q_tree.txt\n");
        return 1;
        }

    
    int n_point_now;        // счетчик вновь созданных точек
    int n_point_old;        // число точек в исходном дереве
    
    int n_point_t0;
    int n_point_t1;
    int n_point_t2;
    int n_point_t3;
    int n_point_t4;
    int n_point_end;
    char first_child_t0[17];
    char first_child_t1[17];
    char first_child_t2[17];
    char first_child_t3[17];
    char first_child_t4[17];
    char first_child_end[17];
    char parent_t0[17];
    char parent_t1[17];
    char parent_t2[17];
    char parent_t3[17];
    char parent_t4[17];
    char parent_end[17];
    int g_number_t0;
    int g_number_t1;
    int g_number_t2;
    int g_number_t3;
    int g_number_t4;
    int g_number_end;
    char point_id_t0[17];
    char point_id_t1[17];
    char point_id_t2[17];
    char point_id_t3[17];
    char point_id_t4[17];
    char point_id_end[17];
    char point_id_new[17];
    double p1_t0;
    double p1_t1;
    double p1_t2;
    double p1_t3;
    double p1_t4;
    double p1_end;
    double p2_t0;
    double p2_t1;
    double p2_t2;
    double p2_t3;
    double p2_t4;
    double p2_end;
    char calculated_t0[2];
    char calculated_t1[2];
    char calculated_t2[2];
    char calculated_t3[2];
    char calculated_t4[2];
    char calculated_end[2];
    double f1_t0;
    double f1_t1;
    double f1_t2;
    double f1_t3;
    double f1_t4;
    double f1_end;
    double f2_t0;
    double f2_t1;
    double f2_t2;
    double f2_t3;
    double f2_t4;
    double f3_t0;
    double f3_t1;
    double f3_t2;
    double f3_t3;
    double f3_t4;
    
    // Создаём временную копию файла q_tree.txt

    FILE *q_tree_temp;

        if((q_tree_temp = fopen("q_tree_temp.txt","w")) == NULL)
        {
        printf("Error creation file q_tree_temp.txt\n");
        return 1;
        }

        while (fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t0, first_child_t0, parent_t0, &g_number_t0, point_id_t0, &p1_t0, &p2_t0, calculated_t0, &f1_t0, &f2_t0, &f3_t0) != EOF)
            {
                fprintf (q_tree_temp, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t0, first_child_t0, parent_t0, g_number_t0, point_id_t0, p1_t0, p2_t0, calculated_t0, f1_t0, f2_t0, f3_t0);
            };
        
        n_point_old = n_point_t0;
        n_point_now = n_point_t0;
        fclose(q_tree_temp);
        fclose(q_tree);    

    // последовательно читаем из файла q_tree.txt строки 
    // нас интересуют только те, номер поколения которых меньше последнего g_namber_in на единицу
    // и при этом у них есть дети

    if((q_tree = fopen(filename3,"r")) == NULL)
        {
            printf("Error opening file q_tree.txt\n");
            return 1;
        }

    // создаём и открываем для записи временный файл, в который будем писать новые строки    

    FILE *q_tree_temp2;

    if((q_tree_temp2 = fopen("q_tree_temp2.txt","w")) == NULL)
                {
                printf("Error creation file q_tree_temp2.txt\n");
                return 1;
                };

    // создаём и открываем для записи временный файл, в который будем писать копии строк, у которых появвятся дети.
    // отличие только в появлении ссылки на первого ребенка    

    FILE *q_tree_temp3;

    if((q_tree_temp3 = fopen("q_tree_temp3.txt","w")) == NULL)
                {
                printf("Error creation file q_tree_temp3.txt\n");
                return 1;
                };

    while (fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t0, first_child_t0, parent_t0, &g_number_t0, point_id_t0, &p1_t0, &p2_t0, calculated_t0, &f1_t0, &f2_t0, &f3_t0) != EOF)
        
        {   
            if ((strcmp(first_child_t0,"0000000000000000") != 0) && (g_number_t0 == g_number_in - 1))
            {
                // Для каждого "не листа" (есть дети) номер поколения которого меньше последнего g_namber_in на единицу
                // необходимо найти его детей и считать их данные
                // для этого используем указатель на первого ребенка, считаем что детей всегда четыре
                // и данные о них записаны в последовательных строках файла q_tree хотя читаем их из его копии
                // для надежности у каждого ребенка проверяем идентификатор родителя
                // Для каждой пятерки родитель + четверо детей выполняем анализ поведения функции распределения
                // это делается последовательно для каждого ребенка
                // если поведение функции распределения "не гладкое" с точки зрения этого ребенка,
                // делаем его родителем и генерируем четыре ребёнка вокруг него
                // данные детей нового поколения пишем последовательно во временный файл q_tree_temp2
                // если статус ребенка текущего поколения поменялся, строку с его полным набором характеристик
                // пишем во временный файл q_tree_temp3

                if((q_tree_temp = fopen("q_tree_temp.txt","r")) == NULL)
                {
                printf("Error opening file q_tree_temp.txt\n");
                return 1;
                };

                while (fscanf (q_tree_temp, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t1, first_child_t1, parent_t1, &g_number_t1, point_id_t1, &p1_t1, &p2_t1, calculated_t1, &f1_t1, &f2_t1, &f3_t1) != EOF)
                    {  
                        if ((strcmp(point_id_t1, first_child_t0) == 0))
                            {
                                fscanf (q_tree_temp, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t2, first_child_t2, parent_t2, &g_number_t2, point_id_t2, &p1_t2, &p2_t2, calculated_t2, &f1_t2, &f2_t2, &f3_t3);
                                // printf("point_id_t2 = %s\n", point_id_t2);
                                fscanf (q_tree_temp, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t3, first_child_t3, parent_t3, &g_number_t3, point_id_t3, &p1_t3, &p2_t3, calculated_t3, &f1_t3, &f2_t3, &f3_t3);
                                // printf("point_id_t3 = %s\n", point_id_t3);
                                fscanf (q_tree_temp, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t4, first_child_t4, parent_t4, &g_number_t4, point_id_t4, &p1_t4, &p2_t4, calculated_t4, &f1_t4, &f2_t4, &f3_t4);
                                // printf("point_id_t4 = %s\n", point_id_t4);
                                fclose(q_tree_temp); 

                               if ((f1_t1 < f_min) && (f1_t0 > f_min) || (f1_t1 > f_min) && (f1_t0 < f_min) || ((f1_t1 > f_min) && (f1_t0 > f_min) && fabs((((f1_t1 + f1_t4) / 2. - f1_t0) / f1_t0)) > allowable_error))
                                    {
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp3, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t1, strncat(strcat(strncpy(point_id_new, point_id_t1, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), point_id_t0, g_number_t1, point_id_t1, p1_t1 , p2_t1, calculated_t1, f1_t1, f2_t1, f3_t1);          
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t1, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t1, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), p1_t1 - size_0 / Pow_int(2,g_number_in + 2), p2_t1 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t1, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t1, g_number_in + 1),"2"), "0000000000000000", 14 - g_number_in), p1_t1 + size_0 / Pow_int(2,g_number_in + 2), p2_t1 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t1, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t1, g_number_in + 1),"3"), "0000000000000000", 14 - g_number_in), p1_t1 - size_0 / Pow_int(2,g_number_in + 2), p2_t1 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t1, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t1, g_number_in + 1),"4"), "0000000000000000", 14 - g_number_in), p1_t1 + size_0 / Pow_int(2,g_number_in + 2), p2_t1 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
                                    };          

                                if ((f1_t2 < f_min) && (f1_t0 > f_min) || (f1_t2 > f_min) && (f1_t0 < f_min) || ((f1_t2 > f_min) && (f1_t0 > f_min) && fabs((((f1_t2 + f1_t3) / 2. - f1_t0) / f1_t0)) > allowable_error))
                                    { 
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp3, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t2, strncat(strcat(strncpy(point_id_new, point_id_t2, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), point_id_t0, g_number_t2, point_id_t2, p1_t2 , p2_t2, calculated_t2, f1_t2, f2_t2, f3_t2);                    
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t2, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t2, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), p1_t2 - size_0 / Pow_int(2,g_number_in + 2), p2_t2 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t2, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t2, g_number_in + 1),"2"), "0000000000000000", 14 - g_number_in), p1_t2 + size_0 / Pow_int(2,g_number_in + 2), p2_t2 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t2, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t2, g_number_in + 1),"3"), "0000000000000000", 14 - g_number_in), p1_t2 - size_0 / Pow_int(2,g_number_in + 2), p2_t2 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t2, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t2, g_number_in + 1),"4"), "0000000000000000", 14 - g_number_in), p1_t2 + size_0 / Pow_int(2,g_number_in + 2), p2_t2 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
                                    };
                                    if ((f1_t3 < f_min) && (f1_t0 > f_min) || (f1_t3 > f_min) && (f1_t0 < f_min) || ((f1_t3 > f_min) && (f1_t0 > f_min) && fabs((((f1_t2 + f1_t3) / 2. - f1_t0) / f1_t0)) > allowable_error))
                                    {
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp3, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t3, strncat(strcat(strncpy(point_id_new, point_id_t3, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), point_id_t0, g_number_t3, point_id_t3, p1_t3 , p2_t3, calculated_t3, f1_t3, f2_t3, f3_t3);                     
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t3, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t3, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), p1_t3 - size_0 / Pow_int(2,g_number_in + 2), p2_t3 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t3, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t3, g_number_in + 1),"2"), "0000000000000000", 14 - g_number_in), p1_t3 + size_0 / Pow_int(2,g_number_in + 2), p2_t3 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t3, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t3, g_number_in + 1),"3"), "0000000000000000", 14 - g_number_in), p1_t3 - size_0 / Pow_int(2,g_number_in + 2), p2_t3 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t3, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t3, g_number_in + 1),"4"), "0000000000000000", 14 - g_number_in), p1_t3 + size_0 / Pow_int(2,g_number_in + 2), p2_t3 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
                                    };
                                    if ((f1_t4 < f_min) && (f1_t0 > f_min) || (f1_t4 > f_min) && (f1_t0 < f_min) || ((f1_t4 > f_min) && (f1_t0 > f_min) && fabs((((f1_t4 + f1_t1) / 2. - f1_t0) / f1_t0)) > allowable_error))
                                    {
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp3, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t4, strncat(strcat(strncpy(point_id_new, point_id_t4, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), point_id_t0, g_number_t4, point_id_t4, p1_t4 , p2_t4, calculated_t4, f1_t4, f2_t4, f3_t4);                     
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t4, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t4, g_number_in + 1),"1"), "0000000000000000", 14 - g_number_in), p1_t4 - size_0 / Pow_int(2,g_number_in + 2), p2_t4 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
    n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t4, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t4, g_number_in + 1),"2"), "0000000000000000", 14 - g_number_in), p1_t4 + size_0 / Pow_int(2,g_number_in + 2), p2_t4 - size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t4, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t4, g_number_in + 1),"3"), "0000000000000000", 14 - g_number_in), p1_t4 - size_0 / Pow_int(2,g_number_in + 2), p2_t4 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
     n_point_now = n_point_now + 1;
    strncpy(point_id_new, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",g_number_in + 2);
    fprintf (q_tree_temp2, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_now, "0000000000000000", point_id_t4, g_number_t0 + 2, strncat(strcat(strncpy(point_id_new, point_id_t4, g_number_in + 1),"4"), "0000000000000000", 14 - g_number_in), p1_t4 + size_0 / Pow_int(2,g_number_in + 2), p2_t4 + size_0 / Pow_int(2,g_number_in + 2), "0", 0.0, 0.0, 0.0);
                                    };
                               break;
                            };
                        };
            };
        };
    
    fclose(q_tree);
    fclose(q_tree_temp2);
    fclose(q_tree_temp3);

    // Для детей, ставших родителями, необходимо актуализировать данные в q_tree используя содержимое q_tree_temp3
    // Читаем строки из q_tree_temp3 и q_tree сливая результат во временный файл q_tree_temp4
    // который затем переименовываем в q_tree

    // printf("CurrentDirectory = %s\n",  argv[0]);
    // printf("CurrentDirectory = %s\n",  get_current_dir_name());

    if((q_tree_temp3 = fopen("q_tree_temp3.txt","r")) == NULL)
        {
        printf("Error creation file q_tree_temp.txt\n");
        return 1;
        }

    if((q_tree = fopen(filename3,"r")) == NULL)
        {
        printf("Error opening file q_tree.txt\n");
        return 1;
        }

    FILE *q_tree_temp4;

    if((q_tree_temp4 = fopen("q_tree_temp4.txt","w")) == NULL)
        {
        printf("Error creation file q_tree_temp.txt\n");
        return 1;
        }

        while (fscanf (q_tree_temp3, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t0, first_child_t0, parent_t0, &g_number_t0, point_id_t0, &p1_t0, &p2_t0, calculated_t0, &f1_t0, &f2_t0, &f3_t0) != EOF)
            {  
                // printf("n_point_t0 = %d\n", n_point_t0);
                do
                    {fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t1, first_child_t1, parent_t1, &g_number_t1, point_id_t1, &p1_t1, &p2_t1, calculated_t1, &f1_t1, &f2_t1, &f3_t1);
                        // printf("n_point_t1 = %d\n", n_point_t1);
                        if (n_point_t0 > n_point_t1)
                        {
                            fprintf (q_tree_temp4, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t1, first_child_t1, parent_t1, g_number_t1, point_id_t1, p1_t1, p2_t1, calculated_t1, f1_t1, f2_t1, f3_t1);
                        }
                        else
                        {
                            fprintf (q_tree_temp4, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t1, first_child_t0, parent_t1, g_number_t1, point_id_t1, p1_t1, p2_t1, calculated_t1, f1_t1, f2_t1, f3_t1);
                        };
             
                    } 
                while (n_point_t0 > n_point_t1);
            };

            while (fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t1, first_child_t1, parent_t1, &g_number_t1, point_id_t1, &p1_t1, &p2_t1, calculated_t1, &f1_t1, &f2_t1, &f3_t1) != EOF)
            {  
                // printf("n_point_t1 = %d\n", n_point_t1);
                fprintf (q_tree_temp4, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t1, first_child_t1, parent_t1, g_number_t1, point_id_t1, p1_t1, p2_t1, calculated_t1, f1_t1, f2_t1, f3_t1);
            };

    fclose(q_tree);
    fclose(q_tree_temp3);
    fclose(q_tree_temp4);

    // удаляем нстарую версию q_tree, ненужный уже q_tree_temp3
   
    remove("q_tree.txt");   
    remove("q_tree_temp3.txt");

    // Переносим все из q_tree_temp4 в q_tree и удаляем временный файл q_tree_temp4

     if((q_tree_temp4 = fopen("q_tree_temp4.txt","r")) == NULL)
        {
        printf("Error creation file q_tree_temp4.txt\n");
        return 1;
        }

     if((q_tree = fopen(filename3,"a")) == NULL)
        {
        printf("Error opening file q_tree.txt\n");
        return 1;
        }

        while (fscanf (q_tree_temp4, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t0, first_child_t0, parent_t0, &g_number_t0, point_id_t0, &p1_t0, &p2_t0, calculated_t0, &f1_t0, &f2_t0, &f3_t0) != EOF)
            {
               fprintf (q_tree, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t0, first_child_t0, parent_t0, g_number_t0, point_id_t0, p1_t0, p2_t0, calculated_t0, f1_t0, f2_t0, f3_t0);
            };

    fclose(q_tree);
    fclose(q_tree_temp4);

    remove("q_tree_temp4.txt");

    // Переносим все новые узлы из q_tree_temp2 в q_tree и удаляем временный файл

    if((q_tree_temp2 = fopen("q_tree_temp2.txt","r")) == NULL)
        {
        printf("Error creation file q_tree_temp2.txt\n");
        return 1;
        }

     if((q_tree = fopen(filename3,"a")) == NULL)
        {
        printf("Error opening file q_tree.txt\n");
        return 1;
        }

        while (fscanf (q_tree_temp2, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t0, first_child_t0, parent_t0, &g_number_t0, point_id_t0, &p1_t0, &p2_t0, calculated_t0, &f1_t0, &f2_t0, &f3_t0) != EOF)
            {
               fprintf (q_tree, "%d %s %s %d %s %.6f %.6f %s %e %e %e \n", n_point_t0, first_child_t0, parent_t0, g_number_t0, point_id_t0, p1_t0, p2_t0, calculated_t0, f1_t0, f2_t0, f3_t0);
            };

    fclose(q_tree);
    fclose(q_tree_temp2);

    
    if (n_point_now > n_point_old)
            {
             printf("Добавлено новых точек = %d\n", n_point_now - n_point_old);
            }
    else    
            {
             printf("Новые точки не добавлялись\n");
            };

    remove("q_tree_temp.txt");   
    remove("q_tree_temp2.txt");    

    // редактируем файл с глобальными параметрами


    int n_point_t5;
    int n_point_good;     
            
    if((task_globe = fopen(filename2,"r")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }
    
    fscanf (task_globe, "%d %d %lf %d %d", &n_point_t5, &n_point_good, &double_term_2, &int_temp_2, &int_temp_2);

    fclose(task_globe);

    if((task_globe = fopen(filename2,"w")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }

    fprintf (task_globe, "%d %d %e %d %d", n_point_t5, n_point_good, double_term_2, g_number_in + 1, n_point_now - n_point_old);

    fclose(task_globe);
       
    return 0;
}