/****************************************************************************************
*	Программа для вычисления функции распределения носителей заряда			*
*	в графене по результатам действия импульсного электрического поля.		*
*	Импульс поля представляет собой суперпозицию двух задаваемых			*
*	независимо ортогональных компонент						*
*	E1(t)=Ea1*Cos(w1*t+fi1)*exp(-(t-t01)^2/(2*(sIgma1/w1)^2)),			*
*	E2(t)=Ea2*Cos(w2*t+fi2)*exp(-(t-t02)^2/(2*(sIgma2/w2)^2)).			*
*	Каждая из компонент имеет форму гармонических колебаний,			*
*	под обрезающей  Гауссовой огибающей.						*
*	При этом для каждой компоненты можно задать собственную амплитуду,		*
*	частоту, фазовый сдвиг, положение во времени максимума обрезающей		*
*	экспоненты и её ширину.								*
*											*
*	Программа использует MPI для распараллеливания.					*
*	За счет распараллеливания функция распределения вычисляется одновременно 	*
*	в нескольких точках импульсного пространства.					*
*	В файле задания определяется прямоугольная область в импульсном			*
*	пространстве, для которой надо получить результат, и число точек по каждой	*
*	компоненте импульса отдельно.							*
*****************************************************************************************/

// Программа берет параметры задачи из файла task_q_easy
// Функция распределения и вспомогательные функции вычисляются для конечного момента времени
// для списка точек в импульсном пространстве.
// Список точек берется из файла q_tree_easy
// Берутся только точки, для которых установлен признак "не посчитано"
// Результаты пишутся во временный файл calc_resalt_temp.txt
// каждой точке соответствует уникальный номер, по которой она затем будет заноситься в task_q_easy


#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>
#include <omp.h>
#include <string.h>


/* Введем структуру для передачи параметров */
struct params_ {
    double Ea1;
    double Ea2;
    double w1;
    double w2;
    double fi1;
    double fi2;
    double t01;
    double t02;
    double sIgma1;
    double sIgma2;
    double p1_loc;
    double p2_loc;
};

/* Дополнительная структура параметров */
struct point {
    int n_point_t;
    char *first_child_t;
    char *parent_t;
    int g_number_t;
    char *point_id_t;
    double p1_t;
    double p2_t;
    char *calculated_t;
    double f1_t;
    double f2_t;
    double f3_t;
};
/* Стуктура для точек отрисовки */
struct draw_point {
    double p1_t;
    double p2_t;
    double f1_t;
    int g_number_t;
};

// Названия файлов для результатов и отрисовки точек.
char *calc_filename = "results/calc_result";
char *draw_filename = "results/dots_for_draw";

/*Определяем зависимость компонент электрического поля от времени*/
double
ePole1_ot_t(double t, void *params) {
    struct params_ calc_params = *(struct params_ *) params;
    double Ea1 = calc_params.Ea1;
    double w1 = calc_params.w1;
    double fi1 = calc_params.fi1;
    double t01 = calc_params.t01;
    double sIgma1 = calc_params.sIgma1;

    return Ea1 * cos(w1 * t + fi1) * exp(-(t - t01) * (t - t01) * w1 * w1 / (2.0 * sIgma1 * sIgma1));
}

double
ePole2_ot_t(double t, void *params) {
    struct params_ parametres = *(struct params_ *) params;
    double Ea2 = parametres.Ea2;
    double w2 = parametres.w2;
    double fi2 = parametres.fi2;
    double t02 = parametres.t02;
    double sIgma2 = parametres.sIgma2;

    return Ea2 * cos(w2 * t + fi2) * exp(-(t - t02) * (t - t02) * w2 * w2 / (2.0 * sIgma2 * sIgma2));
}

/* Определяем производную по времени от электрического поля */
double
ePole1_ot_t_proizvodnaya(double t, void *params) {
    struct params_ parametres = *(struct params_ *) params;
    double Ea1 = parametres.Ea1;
    double w1 = parametres.w1;
    double fi1 = parametres.fi1;
    double t01 = parametres.t01;
    double sIgma1 = parametres.sIgma1;

    return -Ea1 * w1 * exp(-(t - t01) * (t - t01) * w1 * w1 / (2.0 * sIgma1 * sIgma1)) *
           ((t - t01) * w1 * cos(w1 * t + fi1) / (sIgma1 * sIgma1) + sin(w1 * t + fi1));
}

double
ePole2_ot_t_proizvodnaya(double t, void *params) {
    struct params_ parametres = *(struct params_ *) params;
    double Ea2 = parametres.Ea2;
    double w2 = parametres.w2;
    double fi2 = parametres.fi2;
    double t02 = parametres.t02;
    double sIgma2 = parametres.sIgma2;

    return -Ea2 * w2 * exp(-(t - t02) * (t - t02) * w2 * w2 / (2.0 * sIgma2 * sIgma2)) *
           ((t - t02) * w2 * cos(w2 * t + fi2) / (sIgma2 * sIgma2) + sin(w2 * t + fi2));
}

/*Определяем вспомогательную функцию (энергия частицы в поле волны) */
double
ePsilon(double a1_vekt_potencial, double a2_vekt_potencial, void *params) {
    struct params_ parametres = *(struct params_ *) params;
    double p1_loc = parametres.p1_loc;
    double p2_loc = parametres.p2_loc;

    return sqrt(
            p1_loc * p1_loc - 2 * p1_loc * a1_vekt_potencial + a1_vekt_potencial * a1_vekt_potencial + p2_loc * p2_loc -
            2 * p2_loc * a2_vekt_potencial + a2_vekt_potencial * a2_vekt_potencial);
}

/*Определяем вспомогательную функцию Лямбда */
double
Lyambda(double t, double a1_vekt_potencial, double a2_vekt_potencial, void *params) {
    struct params_ parametres = *(struct params_ *) params;
    double p1_loc = parametres.p1_loc;
    double p2_loc = parametres.p2_loc;

    return (ePole1_ot_t(t, &parametres) * (p2_loc - a2_vekt_potencial) -
            ePole2_ot_t(t, &parametres) * (p1_loc - a1_vekt_potencial)) /
           (ePsilon(a1_vekt_potencial, a2_vekt_potencial, &parametres) *
            ePsilon(a1_vekt_potencial, a2_vekt_potencial, &parametres));
}

int
func(double t, const double y[], double f[],
     void *params) {
    struct params_ parametres = *(struct params_ *) params;

    f[0] = 0.5 * Lyambda(t, y[3], y[4], &parametres) * y[1];
    f[1] = Lyambda(t, y[3], y[4], &parametres) * (1.0 - 2.0 * y[0]) - 2.0 * ePsilon(y[3], y[4], &parametres) * y[2];
    f[2] = 2.0 * ePsilon(y[3], y[4], &parametres) * y[1];
    f[3] = -ePole1_ot_t(t, &parametres);
    f[4] = -ePole2_ot_t(t, &parametres);

    return GSL_SUCCESS;
}

int
jac(double t, const double y[], double *dfdy,
    double dfdt[], void *params) {
    struct params_ parameters = *(struct params_ *) params;
    double p1_loc = parameters.p1_loc;
    double p2_loc = parameters.p2_loc;

    gsl_matrix_view dfdy_mat
            = gsl_matrix_view_array(dfdy, 5, 5);
    gsl_matrix *m = &dfdy_mat.matrix;

    gsl_matrix_set(m, 0, 0, 0.0);
    gsl_matrix_set(m, 0, 1, 0.5 * Lyambda(t, y[3], y[4], &parameters));
    gsl_matrix_set(m, 0, 2, 0.0);
    gsl_matrix_set(m, 0, 3, 0.0);
    gsl_matrix_set(m, 0, 4, 0.0);
    gsl_matrix_set(m, 1, 0, -2.0 * Lyambda(t, y[3], y[4], &parameters));
    gsl_matrix_set(m, 1, 1, 0.0);
    gsl_matrix_set(m, 1, 2, -2.0 * ePsilon(y[3], y[4], &parameters));
    gsl_matrix_set(m, 1, 3, 0.0);
    gsl_matrix_set(m, 1, 4, 0.0);
    gsl_matrix_set(m, 2, 0, 0.0);
    gsl_matrix_set(m, 2, 1, -2.0 * ePsilon(y[3], y[4], &parameters));
    gsl_matrix_set(m, 2, 2, 0.0);
    gsl_matrix_set(m, 2, 3, 0.0);
    gsl_matrix_set(m, 2, 4, 0.0);
    gsl_matrix_set(m, 3, 0, 0.0);
    gsl_matrix_set(m, 3, 1, 0.0);
    gsl_matrix_set(m, 3, 2, 0.0);
    gsl_matrix_set(m, 3, 3, 0.0);
    gsl_matrix_set(m, 3, 4, 0.0);
    gsl_matrix_set(m, 4, 0, 0.0);
    gsl_matrix_set(m, 4, 1, 0.0);
    gsl_matrix_set(m, 4, 2, 0.0);
    gsl_matrix_set(m, 4, 3, 0.0);
    gsl_matrix_set(m, 4, 4, 0.0);

    dfdt[0] = (-Lyambda(t, y[3], y[4], &parameters) *
               (ePole1_ot_t(t, &parameters) * (p2_loc - y[4]) - ePole2_ot_t(t, &parameters) * (p1_loc - y[3])) /
               (ePsilon(y[3], y[4], &parameters) * ePsilon(y[3], y[4], &parameters)) +
               (ePole1_ot_t_proizvodnaya(t, &parameters) * (p2_loc - y[4]) -
                ePole2_ot_t_proizvodnaya(t, &parameters) * (p1_loc - y[3])) /
               (2. * ePsilon(y[3], y[4], &parameters) * ePsilon(y[3], y[4], &parameters))) * y[1];
    dfdt[1] = 2.0 * (-Lyambda(t, y[3], y[4], &parameters) *
                     (ePole1_ot_t(t, &parameters) * (p2_loc - y[4]) - ePole2_ot_t(t, &parameters) * (p1_loc - y[3])) /
                     (ePsilon(y[3], y[4], &parameters) * ePsilon(y[3], y[4], &parameters)) +
                     (ePole1_ot_t_proizvodnaya(t, &parameters) * (p2_loc - y[4]) -
                      ePole2_ot_t_proizvodnaya(t, &parameters) * (p1_loc - y[3])) /
                     (2. * ePsilon(y[3], y[4], &parameters) * ePsilon(y[3], y[4], &parameters))) * (1. - 2. * y[0]) -
              2.0 * (((p1_loc - y[3]) * ePole1_ot_t(t, &parameters) + (p2_loc - y[4]) * ePole2_ot_t(t, &parameters)) /
                     ePsilon(y[3], y[4], &parameters)) * y[2];
    dfdt[2] = 2.0 * (((p1_loc - y[3]) * ePole1_ot_t(t, &parameters) + (p2_loc - y[4]) * ePole2_ot_t(t, &parameters)) /
                     ePsilon(y[3], y[4], &parameters)) * y[1];
    dfdt[3] = -ePole1_ot_t_proizvodnaya(t, &parameters);
    dfdt[4] = -ePole2_ot_t_proizvodnaya(t, &parameters);

    return GSL_SUCCESS;
}

int ode_calc(int g_number_t, int n_point_tt, double p1_l, double p2_l, double ePA1, double ePA2, double cP1,
        double cP2, double sI1, double sI2, double t0p1, double t0p2, double fip1, double fip2, double t_s, double t_e) {

    struct params_ calc_params;
    calc_params.Ea1 = ePA1;
    calc_params.Ea2 = ePA2;
    calc_params.w1 = cP1;
    calc_params.w2 = cP2;
    calc_params.fi1 = fip1;
    calc_params.fi2 = fip2;
    calc_params.t01 = t0p1;
    calc_params.t02 = t0p2;
    calc_params.sIgma1 = sI1;
    calc_params.sIgma2 = sI2;
    calc_params.p1_loc = p1_l;
    calc_params.p2_loc = p2_l;

    gsl_odeiv2_system sys = {func, jac, 5, &calc_params};

    gsl_odeiv2_driver *d =
            gsl_odeiv2_driver_alloc_y_new(&sys, gsl_odeiv2_step_rk8pd, 1e-14, 1e-14, 0.0);

    double t = t_s, t1 = t_e - t_s;

    double y[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

    double ti = t + t1 / 1.0;

    int status = gsl_odeiv2_driver_apply(d, &t, ti, y);

    if (status != GSL_SUCCESS) {
        printf("error, return value=%d\n", status);
        return 0;
    }


    FILE *calc_result_file;

    if ((calc_result_file = fopen(calc_filename, "a")) == NULL) {
        fprintf(stderr,"Error opening file '%s'\n", calc_filename);
        return 1;
    }


    FILE *draw_points_file = fopen(draw_filename, "a");
    if (!draw_points_file) {
        fprintf(stderr, "Error opening file '%s'\n", draw_filename);
        return 0;
    }

    // Запись точек для отрисовки в файл
    fprintf(draw_points_file, "%.10f %.10f %.15e %d\n", p1_l, p2_l, y[0], g_number_t);
    fclose(draw_points_file);

    // Запись результатов в файл
    fprintf(calc_result_file, "%d %.10f %.10f  %.10e %.10e  %.10e\n", n_point_tt, p1_l, p2_l, y[0], y[1], y[2]);
    fclose(calc_result_file);


    gsl_odeiv2_driver_free(d);

}

int md_comparator(const void *v1, const void *v2)
{
    const struct draw_point *p1 = (struct draw_point *)v1;
    const struct draw_point *p2 = (struct draw_point *)v2;
    if(p1->g_number_t < p2->g_number_t)
        return -1;
    else if(p1->g_number_t > p2->g_number_t)
        return +1;
    else if (p1->p1_t < p2->p1_t)
        return -1;
    else if (p1->p1_t > p2->p1_t)
        return +1;
    else if (p1->p2_t < p2->p2_t)
        return -1;
    else if (p1->p2_t > p2->p2_t)
        return +1;
    else
        return 0;
}

int sort_dots_for_graphs (struct draw_point* all_points, int len) {


    FILE *draw_points_file = fopen(draw_filename, "r");
    if (!draw_points_file) {
        fprintf(stderr, "Error opening file '%s'\n", draw_filename);
        return 0;
    }

    double y0;
    double p1_t;
    double p2_t;
    int g_number_t;

    for (int i = 0; i < len; ++i) {
        fscanf(draw_points_file, "%lf %lf %lf %d", &p1_t, &p2_t, &y0, &g_number_t);
        all_points[i].f1_t = y0;
        all_points[i].p1_t = p1_t;
        all_points[i].p2_t = p2_t;
        all_points[i].g_number_t = g_number_t;
    }
    fclose(draw_points_file);

    // Сортировка данных
    qsort(all_points, len, sizeof(struct draw_point), md_comparator);

    // Создаем файл для сортированных точек
    FILE *draw_points_file_sorted = fopen("results/dots_for_draw_sorted", "w");
    fprintf(draw_points_file_sorted, "X Y Z\n");


    for (int i = 0; i < len; ++i) {
        fprintf(draw_points_file_sorted, "%.10f %.10f %.20f %d\n",
                all_points[i].p1_t, all_points[i].p2_t, all_points[i].f1_t, all_points[i].g_number_t);
    }
    fclose(draw_points_file_sorted);
}

int draw_graphs(struct draw_point* all_points, int len) {

    // Отрисовываем трехмерный график в файл 3d_result.png
    char * commandsForGnuplot[] =
            {"set title \"3D graph of assign points\"",
             "set term png",
             "set output \"results/3d_result.png\"",
             "set palette defined (0 \"green\", 1 \"black\")",
             "splot 'results/dots_for_draw_sorted' using 1:2:3 with "
             "points pointtype 7 pointsize 1 lc palette"};

    int num_commands = (sizeof(commandsForGnuplot) / sizeof((commandsForGnuplot)[0]));

    // Открываем постоянное окно gnuplot
    FILE* gnuplot_pipe = popen ("gnuplot -persistent", "w");

    // Выполняем все команды
    for(int i = 0; i < num_commands; ++i){
        fprintf(gnuplot_pipe, "%s \n", commandsForGnuplot[i]);
    }
    fclose(gnuplot_pipe);
    // Получаем максимальный уровень поколения
    int max_level_generation = all_points[len - 1].g_number_t;
    char graph_gen_filename[50];

    // Отрисовываем графики по поколениям
    // Создаем файлы для каждого покления в папке 'results/gens'
    for(int i = 1; i <= max_level_generation; ++i) {
        int gen_number = i;
        char generation_draw_filename[30] = "results/gens/generation_";
        char gen_number_str[3];
        sprintf(gen_number_str, "%d", gen_number);
        strcat(generation_draw_filename, gen_number_str);

        FILE *generation_draw_file = fopen(generation_draw_filename, "w");
        if (!generation_draw_file) {
            fprintf(stderr, "Error opening file '%s'\n", draw_filename);
            return 0;
        }

        for(int j = 0; j < len; ++j) {
            if(all_points[j].g_number_t == gen_number) {
                fprintf(generation_draw_file, "%.10f %.10f %.20f %d\n",
                        all_points[j].p1_t, all_points[j].p2_t, all_points[j].f1_t, all_points[j].g_number_t);
            }
        }
        fclose(generation_draw_file);

        FILE* gnuplot_pipe_generations = popen ("gnuplot -persistent", "w");

        // Делаем массив пустым
        memset(graph_gen_filename, 0, sizeof graph_gen_filename);

        strcat(graph_gen_filename, "set output \"");
        strcat(graph_gen_filename, generation_draw_filename);
        strcat(graph_gen_filename, ".png\"");

        // Выполняем все команды для отрисовки
        fprintf(gnuplot_pipe_generations, "%s %s %s\n", "set title \"3D chart of", generation_draw_filename, "\"");
        fprintf(gnuplot_pipe_generations, "%s \n","set term png");
        fprintf(gnuplot_pipe_generations, "%s \n", graph_gen_filename);
        fprintf(gnuplot_pipe_generations, "%s \n","set palette defined (0 \"green\", 1 \"black\")");
        fprintf(gnuplot_pipe_generations, "%s%s%s", "splot \'", generation_draw_filename, "\' using 1:2:3 with "
            "points pointtype 7 pointsize 1 lc palette");

        fclose(gnuplot_pipe_generations);
    }
    fclose(gnuplot_pipe);
}



/* Определим набор задаваемых физических параметров:
*
*  Предполагается, что начальное состояние двумерной среды вакуумное
*  Для графена это реализуется при близких к абсолютному нулю температурах.
*
*  Действующее в плоскости моделируемого образца поле представляется
*  суперпозицией двух "радиоимпульсов".
*  они могут иметь собственные частоты, фазовые сдвиги и даже сдвинутые во времени максимумы.
*  Это позволяет рассматривать достаточно разнообразные конфигурации поля.
*
*  Для каждой компоненты задаются амплитуды Ea1 и Ea2
*  В используемой системе единиц значение E = 1.0 соответствует 1.088*10^10 в/м
*  Напряженности поля 10^6 в/м соответствует значение E = 9.19*10^-5
*  При вычислениях используются циклические частоты несущих w1 и w2.
*  Будем определять их через обычные частоты nu1 и nu2, задаваемые в качестве параметров задачи.
*  В используемой системе единиц nu = 1 соответствует 4.065*10^15 Гц.
*  Частоте в 10^12 Гц (1 ТГц) соответствует nu = 2.46*10^-4.
*  Фазовые сдвиги безразмерны и могут задаваться в интервале -pi/2 < fi < pi/2.
*  Смещения максимумов гауссовых огибающих задаются (при необходимости) через параметры t01 и t02,
*  имеющие размерность времени. Их значения предполагаются ограниченными по абсолютной величине
*  примерно обратным значением минимальной из частот nu1 или  nu2.
*  Относительные длительности импульсов определяются параметрами sIgma1 и sIgma2
*  (характерное время нарастания/затухания относительно периода несущей).
*  Эти величины безразмерны. Их значения предполагаются большими примерно 5.
*  В противном случае будет возникать ассимметрия в начальном и конечном значении
*  векторного потенциала, учет которой в данной версии программы не предусмотрен.
*
*  Параметры импульса поля читаются из текстового файла task_q
*  В этот файл они помещаются построчно, каждый пораметр с новой строки. При невозможности
*  найти файл с параметрами задача запущена не будет.
*
*  Данная модификация программы предназначена для работы в составе системы для автоматического
*  построения оптимальной сетки в формате квадродерева.
*
*  Список точек для счета берется из обновляемого файла q_tree
*  Файл организован построчно.
*  В каждой строке хранятся координаты точки, её параметры размещения в квадродереве,
*  признак посчитана/непосчитана и собственно результаты счета.
*
*  Интегрирование для каждой точки выполняется отдельным процессом.
*
*
*  Результаты рассчетов вписываются в соответствующую строку файла.                              */


int
main (int argc, char **argv) {

    double start_time = omp_get_wtime();
    double Ea1;        /*амплитуда 1-й компоненты электрического поля в графеновых единицах*/
    double Ea2;        /*амплитуда 2-й компоненты электрического поля в графеновых единицах*/
    double nu1;        /*частота 1-й компоненты поля в графеновых единицах*/
    double nu2;        /*частота 2-й компоненты поля в графеновых единицах*/
    double fi1;        /*фазовый сдвиг 1-й компоненты поля*/
    double fi2;        /*фазовый сдвиг 2-й компоненты поля*/
    double t01;        /*временной сдвиг максимума 1-й компоненты в графеновых единицах*/
    double t02;        /*временной сдвиг максимума 2-й компоненты в графеновых единицах*/
    double sIgma1;    /*ширина гауссовой огибающей 1-й компоненты*/
    double sIgma2;    /*ширина гауссовой огибающей 2-й компоненты*/
    double t_start;  /*начало интервала интегрирования*/
    double t_end;    /*конец интервала интегрирования*/

    double w1;    // первая циклическая частота
    double w2;    // вторая циклическая частота



    FILE *task_q;
    char s[80];

    if (argc < 3) {
        printf("no files task_q or q_tree\n");
        exit(1);
    }

    char *taskq_filename = argv[1];

    if ((task_q = fopen(taskq_filename, "r")) == NULL) {
        printf("Error opening file %s\n", taskq_filename);
        return 1;
    }

    fscanf(task_q, "%lf %s", &Ea1, s);
    fscanf(task_q, "%lf %s", &Ea2, s);
    fscanf(task_q, "%lf %s", &nu1, s);
    fscanf(task_q, "%lf %s", &nu2, s);
    fscanf(task_q, "%lf %s", &fi1, s);
    fscanf(task_q, "%lf %s", &fi2, s);
    fscanf(task_q, "%lf %s", &t01, s);
    fscanf(task_q, "%lf %s", &t02, s);
    fscanf(task_q, "%lf %s", &sIgma1, s);
    fscanf(task_q, "%lf %s", &sIgma2, s);
    fscanf(task_q, "%lf %s", &t_start, s);
    fscanf(task_q, "%lf %s", &t_end, s);

    // закрываем файл с параметрами задания
    fclose(task_q);

    // вычисляем циклические частоты, которые используются в формулах далее
    w1 = 6.2831853 * nu1;
    w2 = 6.2831853 * nu2;

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
    char *tree_filename = argv[2];


    /* Open the file for count lines */
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    ssize_t line_size = 0;
    FILE *q_tree_0 = fopen(tree_filename, "r");
    if (!q_tree_0) {
        fprintf(stderr, "Error opening file '%s'\n", tree_filename);
        return 0;
    }
    while (line_size >= 0) {
        /* Increment our line count */
        line_count++;
        /* Get the next line */
        line_size = getline(&line_buf, &line_buf_size, q_tree_0);
    }
    line_count -= 1;
    /* Free the allocated line buffer */
    line_buf = NULL;
    /* Close the file now that we are done with it */
    fclose(q_tree_0);

    struct point all_data[line_count];


    // Open file again and read all data to array of structs
    FILE *q_tree = fopen(tree_filename, "r");

    for (int i = 0; i < line_count; ++i) {
        fscanf(q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le",
               &n_point_t, first_child_t, parent_t, &g_number_t, point_id_t, &p1_t,
               &p2_t, calculated_t, &f1_t, &f2_t, &f3_t);

        all_data[i].n_point_t = n_point_t;
        all_data[i].first_child_t = first_child_t;
        all_data[i].parent_t = parent_t;
        all_data[i].g_number_t = g_number_t;
        all_data[i].point_id_t = point_id_t;
        all_data[i].p1_t = p1_t;
        all_data[i].p2_t = p2_t;
        all_data[i].calculated_t = calculated_t;
        all_data[i].f1_t = f1_t;
        all_data[i].f2_t = f2_t;
        all_data[i].f3_t = f3_t;
    }
    fclose(q_tree);

    // Clean files file before writing information
    fclose(fopen(calc_filename, "w"));
    FILE *draw_file = fopen(draw_filename, "w");
//    fprintf(draw_file, "X Y Z\n");
    fclose(draw_file);


    struct draw_point draw_points[line_count];
    // Set threads

    int thread_count = 6;
    omp_set_num_threads(thread_count);
#pragma omp parallel for
    for (int i = 0; i < line_count; ++i) {
        if (calculated_t[0] == '0') {
            ode_calc(all_data[i].g_number_t, all_data[i].n_point_t, all_data[i].p1_t, all_data[i].p2_t, Ea1, Ea2, w1,
                     w2, sIgma1, sIgma2, t01, t02, fi1, fi2, t_start, t_end);
            int thread_num = omp_get_thread_num();
//        printf("Num thread: %i\n", thread_num);
        }
    }
    printf("Расчет выполнен.\n");
    double end_time = omp_get_wtime() - start_time;
    printf("Затраченное время на расчет: %f\n", end_time);
    printf("Кол-во использованных потоков: %d\n",thread_count);
    printf("Сортируем полученные данные и строим графики...\n");
    // Функция сортировки точек
    sort_dots_for_graphs(draw_points, line_count);
    // Функция отрисовки графиков
    draw_graphs(draw_points, line_count);

    return 0;
}