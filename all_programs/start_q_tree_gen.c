/****************************************************************************************
*	Програмный модуль для генерации начального квадродерева покрывающего заданную          *
* квадратную область импульсного пространства полной сеткой без пропусков                          *
* исходные данные беруться из файла task_q.txt с параметрами задачи.                       *
* Максимальный размер генерируемого квадродерева может включать восемь поколений           *
* с 0-вого по 7-е.                                                                         *
* В таком дереве будет всего 21845 элементов                                               *
* В его 7-м поколении будет 16384 элемента, образующих решетку 128 х 128                   *
* Это условие ограничивает отношение параметров задачи size_0 / size_1 <= 128    
* Результат записывается в файл stsrt_q_tree.txt
*         *
*****************************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в его текстовый ID                                                            *
*  который представляет из себя строку из 16 символов                            *
*  первый символ всегда 0, корневому узлу соответствует код 0000000000000000     *
*  в каждой следующей позиции пишется номер квадранта соответствующего поколения */

char *num_to_id (int point_number)
{
    char *resalt = malloc(17);
    resalt[0] = '0';
    resalt[16] = '\0';
    
    char to_resalt[1];

    int num = point_number;
    int g;

    for (g = 1; g <= 16; g++)
        {
            resalt[16-g] = '0';
            if (num > 0)
                {
                    if (num % Pow_int(4,g) == 0)
                        {
                            to_resalt[0] = 4 + '0';
                            num = num - 4 * Pow_int(4,g - 1);
                        }
                    else
                        {
                            to_resalt[0] = (num % Pow_int(4,g)) / Pow_int(4,g - 1) + '0';
                            num = num - ((num % Pow_int(4,g)) / Pow_int(4,g - 1)) * Pow_int(4,g - 1);
                        };
                    resalt[16-g] = to_resalt[0];
                };
        };

//  printf("%s\n", resalt);   

/* Мы получили код в формате 0000000000000xyz            *
*  теперь приведем его к виду 0xyz00000000000            */

    int stp = 1;

    for (g = 1; g <= 40; g++)
        {
            if (resalt[g] == '\0')
                {
                    break;   
                };
            if (resalt[g] != '0')
                {
                    resalt[stp] = resalt[g];
                    stp = stp + 1;
                    resalt[g] = '0';
                };
        };
    return resalt;
//  printf("%s\n", resalt);
}

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в текстовый ID его первого ребенка                                            */

char *num_to_first_child (int point_number, int g_start)
{
    int g;
    char *id = num_to_id (point_number);
    for (g = 2; g <= 16; g++)
        {
            if (g < g_start + 1 && id[g] == '0')
                {
                    id[g] = '1';
                    break;
                }
            else
                {
                    if (g == g_start + 1)
                    {
                        id = "0000000000000000";
                        break;
                    };
                };    
        };
    return id;    
}

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в текстовый ID его родителя                                                   */

char *num_to_parent (int point_number)
{
    int g;
    char *id = num_to_id (point_number);
    for (g = 2; g <= 16; g++)
        {
            if (id[g] == '0')
                {
                    id[g - 1] = '0';
                    break;
                };
        };
    return id;    
}

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в номер его  поколения                                                        */

int num_to_g_num (int point_number)
{
    int g;
    int g_num;
    char *id = num_to_id (point_number);
    for (g = 2; g <= 16; g++)
        {
            if (id[g] == '0')
                {
                    g_num = g - 1;
                    break;
                };
        };
    return g_num;    
}

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в его первую координату                                                       */

double num_to_p1 (int point_number, double p1_zero, double size_0)
{
    int g;
    double p1 = p1_zero;
    double step = size_0 / 4;
    char *id = num_to_id (point_number);
    for (g = 1; g <= 16; g++)
        {
            if (id[g] == '0')
                {
                   break;
                };
            if (id[g] == '1')
                {
                   p1 = p1 - step;
                   step = step / 2;
                };
            if (id[g] == '2')
                {
                   p1 = p1 + step;
                   step = step / 2;
                };
            if (id[g] == '3')
                {
                   p1 = p1 - step;
                   step = step / 2;
                };
            if (id[g] == '4')
                {
                   p1 = p1 + step;
                   step = step / 2;
                };             
        };
    return p1;    
}

/* Определяем функцию, преобразующую порядковый номер узла в полном квадродереве *
*  в его вторую координату                                                       */

double num_to_p2 (int point_number, double p2_zero, double size_0)
{
    int g;
    double p2 = p2_zero;
    double step = size_0 / 4;
    char *id = num_to_id (point_number);
    for (g = 1; g <= 16; g++)
        {
            if (id[g] == '0')
                {
                   break;
                };
            if (id[g] == '1')
                {
                   p2 = p2 - step;
                   step = step / 2;
                };
            if (id[g] == '2')
                {
                   p2 = p2 - step;
                   step = step / 2;
                };
            if (id[g] == '3')
                {
                   p2 = p2 + step;
                   step = step / 2;
                };
            if (id[g] == '4')
                {
                   p2 = p2 + step;
                   step = step / 2;
                };             
        };
    return p2;    
}

int main (int argc, char **argv)
{
//    double Ea1;		/*амплитуда 1-й компоненты электрического поля в графеновых единицах*/
//    double Ea2;		/*амплитуда 2-й компоненты электрического поля в графеновых единицах*/
//    double nu1;		/*частота 1-й компоненты поля в графеновых единицах*/
//    double nu2;		/*частота 2-й компоненты поля в графеновых единицах*/
//    double fi1;		/*фазовый сдвиг 1-й компоненты поля*/
//    double fi2;		/*фазовый сдвиг 2-й компоненты поля*/
//    double t01;		/*временной сдвиг максимума 1-й компоненты в графеновых единицах*/
//    double t02;		/*временной сдвиг максимума 2-й компоненты в графеновых единицах*/
//    double sIgma1;	/*ширина гауссовой огибающей 1-й компоненты*/
//    double sIgma2;	/*ширина гауссовой огибающей 2-й компоненты*/
//    double t_in;      /*начальное время интегрирования уравнения*/
//    double t_out;     /*конечное время интегрирования уравнения*/
//    double t_in_list; /*начальное время записи промежуточных значений*/
//    double t_out_list;/*конечное время записи промежуточных значений*/
//    int steps;        /*число шагов по времени при записи промежуточных значений*/
    double p1_zero;     /*первая координата центра решетки*/
    double p2_zero;     /*вторая координата центра решетки*/
    double size_0;      /*размер покрываемой области*/
    double size_1;      /*размер максимальной ячейки не более*/
//    double size_3;    /*размер минимальной ячейки не более*/
//    double f_min;     /*минимальное значение функции распределения, принимаемое не нулевым*/
//    double error_r;   /*допустимая относительная ошибка при линейной экстраполяции*/
   
   

    /* Проверяем наличие файла с параметрами задания task_q */
    
    FILE *task_q;
    char s[100];
    int int_term;
    double double_term;
    
    if (argc < 2) 
    {
	printf("no files task_q.txt!\n");
	exit(1);
    }
    char *filename1 = argv[1];
    
    if((task_q = fopen(filename1,"r")) == NULL)
    {
    printf("Error opening file task_q.txt\n");
    return 1;
    }
    
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
    fscanf(task_q, "%lf %s", &p1_zero, s);
    fscanf(task_q, "%lf %s", &p2_zero, s);
    fscanf(task_q, "%lf %s", &size_0, s);
    fscanf(task_q, "%lf %s", &size_1, s);
    /*fscanf(task_q, "%lf %s", &double_term, s);*/
    /*fscanf(task_q, "%lf %s", &double_term, s);*/
    /*fscanf(task_q, "%lf %s", &double_term, s);*/
    fclose(task_q);

    printf("p1_zero = %.10f\n", p1_zero);
    printf("p2_zero = %.10f\n", p2_zero);
    printf("size_0 = %.10f\n", size_0);
    printf("size_1 = %.10f\n", size_1);
   /* printf("int_term = %d\n", int_term);
    printf("double_term = %d\n", double_term);
    printf("s = %s\n", s);   */
    
    /* Определим число поколений квадродерева, которые необходимо построить */
    /* Определяем число элементов в этом квадродереве                       */

    int g_start = ceil(log2(size_0/size_1));
    int length_start = ((Pow_int(4, g_start + 1) - 1) / 3);

    printf("g_start = %d\n", g_start);
    /* Проверяем ограничение на число поколений */

    if (g_start > 7) 
    {
    printf("Exceeded size limit for start tree!\n");
    exit(1);
    }

/* Создаем структуру для записи данных по каждой новой точке  */

struct q_point 
    {
    int n_point; 
    char first_child[17];
    char parent[17];
    int g_number;
    char point_id[17];
    double p1;
    double p2;
    char calculated[2];
    };


/* Создаем массив из таких структур                                            */
/* Размер массива позволяет записать в него полное квадродерево из 8 поколений */

struct q_point q_start[21845];

/* Создаем файл q_tree, куда будем построчно писать                            */
/* данные об узлах формируемого квадродерева                                   */
/* Затем этот файл будет использоваться для хранения дополняемого дерева точек */

FILE *mf;

    if((mf = fopen("q_tree.txt","w")) == NULL)
    {
    printf("Error creation file q_tree.txt\n");
    return 1;
    }

/* Формируем корень нашего дерева                                              */

    q_start[0].n_point = 0; 
    strcpy(q_start[0].first_child, "0100000000000000");
    strcpy(q_start[0].parent, "0000000000000000");
    q_start[0].g_number = 0;
    strcpy(q_start[0].point_id, "0000000000000000");
    q_start[0].p1 = p1_zero; 
    q_start[0].p2 = p2_zero;
    strcpy(q_start[0].calculated, "0");

/*printf ("%d %s %s %d %s %.6f %.6f %s \n", q_start[0].n_point, q_start[0].first_child, q_start[0].parent, q_start[0].g_number, q_start[0].point_id, q_start[0].p1, q_start[0].p2, q_start[0].calculated);*/

fprintf (mf, "%d %s %s %d %s %.10f %.10f %s %e %e %e \n", q_start[0].n_point, q_start[0].first_child, q_start[0].parent, q_start[0].g_number, q_start[0].point_id, q_start[0].p1, q_start[0].p2, q_start[0].calculated, 0.0, 0.0, 0.0);


/* Заполняем маассив структур начального полного квадродерева                  */

int num;

    for(num = 1; num < length_start; num++)
    {
       q_start[num].n_point = num; 
       strcpy(q_start[num].first_child, num_to_first_child(num, g_start));
       strcpy(q_start[num].parent, num_to_parent(num));
       q_start[num].g_number = num_to_g_num(num);
       strcpy(q_start[num].point_id, num_to_id(num));
       q_start[num].p1 = num_to_p1(num, p1_zero, size_0);
       q_start[num].p2 =num_to_p2(num, p2_zero, size_0);
       strcpy(q_start[num].calculated, "0");

/* Результат для каждого узла выводим в виде форматированной строки             */
           
       fprintf (mf, "%d %s %s %d %s %.10f %.10f %s %e %e %e \n", q_start[num].n_point, q_start[num].first_child, q_start[num].parent, q_start[num].g_number, q_start[num].point_id, q_start[num].p1, q_start[num].p2, q_start[num].calculated, 0.0, 0.0, 0.0);
    }
    
fclose(mf);

// формируем и заполняем файл с глобальными параметрами

FILE *task_globe;

        if((task_globe = fopen("task_globe.txt","w")) == NULL)
        {
        printf("Error opening file task_globe.txt\n");
        return 1;
        }

        fprintf (task_globe, "%d %d %e %d %d", 0, 0, 0.0, g_start, length_start);

    fclose(task_globe);

    return 0;
}