/****************************************************************************************
*	Программа для вычисления фуккции распределения носителей заряда			*
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

// Программа берет параметры задачи из файла task_q.txt
// Функция распределения и вспомогательные функции вычисляются для конечного момента времени
// для списка точек в импульсном пространстве.
// Список точек берется из файла q_tree.txt
// Берутся только точки, для которых установлен признак "не посчитано"
// Результаты пишутся во временный файл calc_resalt_temp.txt
// каждой точке соответствует уникальный номер, по которой она затем будет заноситься в task_q.txt

#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

/*Введем структуру для передачи параметров*/
struct parametres_{
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

/*Определяем зависимость компонент электрического поля от времени*/
double
ePole1_ot_t (double t, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double Ea1 = parametres.Ea1;
  double w1 = parametres.w1;
  double fi1 = parametres.fi1;
  double t01 = parametres.t01;
  double sIgma1 = parametres.sIgma1;
  
  return Ea1 * cos(w1 * t + fi1) * exp( - (t - t01) * (t - t01) * w1 *w1 / (2.0 * sIgma1 * sIgma1));
}

double
ePole2_ot_t (double t, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double Ea2 = parametres.Ea2;
  double w2 = parametres.w2;
  double fi2 = parametres.fi2;
  double t02 = parametres.t02;
  double sIgma2 = parametres.sIgma2;
  
  return Ea2 * cos(w2 * t + fi2) * exp( - (t - t02) * (t - t02) *w2 * w2 / (2.0 * sIgma2 * sIgma2));
}

/*Определяем производную по времени от электрического поля*/
double
ePole1_ot_t_proizvodnaya (double t, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double Ea1 = parametres.Ea1;
  double w1 = parametres.w1;
  double fi1 = parametres.fi1;
  double t01 = parametres.t01;
  double sIgma1 = parametres.sIgma1;
    
  return - Ea1 * w1 * exp( - (t - t01) * (t - t01) * w1 * w1 / (2.0 * sIgma1 * sIgma1)) * ((t - t01) * w1 * cos(w1 * t + fi1) / (sIgma1 * sIgma1) + sin(w1 * t + fi1));
}

double
ePole2_ot_t_proizvodnaya (double t, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double Ea2 = parametres.Ea2;
  double w2 = parametres.w2;
  double fi2 = parametres.fi2;
  double t02 = parametres.t02;
  double sIgma2 = parametres.sIgma2;
    
  return - Ea2 * w2 * exp( - (t - t02) * (t - t02) * w2 * w2 / (2.0 * sIgma2 * sIgma2)) * ((t - t02) * w2 * cos(w2 * t + fi2) / (sIgma2 * sIgma2) + sin(w2 * t + fi2));
}

/*Определяем вспомогательную функцию (энергия частицы в поле волны) */
double
ePsilon (double a1_vekt_potencial, double a2_vekt_potencial, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double p1_loc = parametres.p1_loc;
  double p2_loc = parametres.p2_loc;

  return sqrt(p1_loc * p1_loc - 2 * p1_loc * a1_vekt_potencial + a1_vekt_potencial * a1_vekt_potencial + p2_loc * p2_loc - 2 * p2_loc * a2_vekt_potencial + a2_vekt_potencial * a2_vekt_potencial);
}

/*Определяем вспомогательную функцию Лямбда */
double
Lyambda (double t,double a1_vekt_potencial, double a2_vekt_potencial, void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double p1_loc = parametres.p1_loc;
  double p2_loc = parametres.p2_loc;
  
  return (ePole1_ot_t(t, &parametres) * (p2_loc - a2_vekt_potencial) - ePole2_ot_t(t, &parametres) * (p1_loc - a1_vekt_potencial))/ (ePsilon(a1_vekt_potencial, a2_vekt_potencial, &parametres) * ePsilon(a1_vekt_potencial, a2_vekt_potencial, &parametres));
}

int
func (double t, const double y[], double f[],
      void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  
  f[0] = 0.5 * Lyambda(t, y[3], y[4], &parametres) * y[1] ;
  f[1] = Lyambda(t, y[3], y[4], &parametres) * (1.0 - 2.0 * y[0]) - 2.0 * ePsilon(y[3], y[4], &parametres) * y[2];
  f[2] = 2.0 * ePsilon(y[3], y[4], &parametres) * y[1];
  f[3] = - ePole1_ot_t(t, &parametres);
  f[4] = - ePole2_ot_t(t, &parametres);
  
  return GSL_SUCCESS;
}

int
jac (double t, const double y[], double *dfdy, 
     double dfdt[], void *params)
{
  struct parametres_ parametres = *(struct parametres_ *)params;
  double Ea1 = parametres.Ea1;
  double Ea2 = parametres.Ea2;
  double w1 = parametres.w1;
  double w2 = parametres.w2;
  double fi1 = parametres.fi1;
  double fi2 = parametres.fi2;
  double t01;
  double t02;
  double sIgma1 = parametres.sIgma1;
  double sIgma2 = parametres.sIgma2;
  double p1_loc = parametres.p1_loc;
  double p2_loc = parametres.p2_loc;
    
  gsl_matrix_view dfdy_mat 
    = gsl_matrix_view_array (dfdy, 5, 5);
  gsl_matrix * m = &dfdy_mat.matrix;
  
  gsl_matrix_set (m, 0, 0, 0.0);
  gsl_matrix_set (m, 0, 1, 0.5 * Lyambda(t, y[3], y[4] ,&parametres));
  gsl_matrix_set (m, 0, 2, 0.0);
  gsl_matrix_set (m, 0, 3, 0.0);
  gsl_matrix_set (m, 0, 4, 0.0);
  gsl_matrix_set (m, 1, 0, - 2.0 * Lyambda(t, y[3], y[4], &parametres));
  gsl_matrix_set (m, 1, 1, 0.0);
  gsl_matrix_set (m, 1, 2, - 2.0 * ePsilon(y[3], y[4], &parametres));
  gsl_matrix_set (m, 1, 3, 0.0);
  gsl_matrix_set (m, 1, 4, 0.0);
  gsl_matrix_set (m, 2, 0, 0.0);
  gsl_matrix_set (m, 2, 1, - 2.0 * ePsilon(y[3], y[4], &parametres));
  gsl_matrix_set (m, 2, 2, 0.0);
  gsl_matrix_set (m, 2, 3, 0.0);
  gsl_matrix_set (m, 2, 4, 0.0);
  gsl_matrix_set (m, 3, 0, 0.0);
  gsl_matrix_set (m, 3, 1, 0.0);
  gsl_matrix_set (m, 3, 2, 0.0);
  gsl_matrix_set (m, 3, 3, 0.0);
  gsl_matrix_set (m, 3, 4, 0.0);
  gsl_matrix_set (m, 4, 0, 0.0);
  gsl_matrix_set (m, 4, 1, 0.0);
  gsl_matrix_set (m, 4, 2, 0.0);
  gsl_matrix_set (m, 4, 3, 0.0);
  gsl_matrix_set (m, 4, 4, 0.0);
  
  dfdt[0] = (- Lyambda(t, y[3], y[4], &parametres) * (ePole1_ot_t(t, &parametres) * (p2_loc - y[4]) - ePole2_ot_t(t, &parametres) * (p1_loc - y[3])) / (ePsilon(y[3], y[4], &parametres) * ePsilon(y[3], y[4], &parametres)) + (ePole1_ot_t_proizvodnaya(t, &parametres) * (p2_loc - y[4]) - ePole2_ot_t_proizvodnaya(t, &parametres) * (p1_loc - y[3])) / (2. *  ePsilon(y[3], y[4], &parametres) * ePsilon(y[3], y[4], &parametres))) * y[1];
  dfdt[1] = 2.0 * (- Lyambda(t, y[3], y[4], &parametres) * (ePole1_ot_t(t, &parametres) * (p2_loc - y[4]) - ePole2_ot_t(t, &parametres) * (p1_loc - y[3])) / (ePsilon(y[3], y[4], &parametres) * ePsilon(y[3], y[4], &parametres)) + (ePole1_ot_t_proizvodnaya(t, &parametres) * (p2_loc - y[4]) - ePole2_ot_t_proizvodnaya(t, &parametres) * (p1_loc - y[3])) / (2. *  ePsilon(y[3], y[4], &parametres) * ePsilon(y[3], y[4], &parametres))) * (1. - 2.*y[0]) - 2.0 * (((p1_loc - y[3]) * ePole1_ot_t (t, &parametres) + (p2_loc - y[4]) * ePole2_ot_t (t, &parametres))/ ePsilon(y[3], y[4], &parametres)) * y[2];
  dfdt[2] = 2.0 * (((p1_loc - y[3]) * ePole1_ot_t (t, &parametres) + (p2_loc - y[4]) * ePole2_ot_t (t, &parametres))/ ePsilon(y[3], y[4], &parametres)) * y[1];
  dfdt[3] = - ePole1_ot_t_proizvodnaya(t, &parametres);
  dfdt[4] = - ePole2_ot_t_proizvodnaya(t, &parametres);
  
  return GSL_SUCCESS;
}

int
ode_print (int n_point_tt, double p1_l, double p2_l, double ePA1, double ePA2, double cP1, double cP2, double sI1, double sI2, double t0p1, double t0p2, double fip1, double fip2, double t_s, double t_e)
{
  struct parametres_ parametres;
  parametres.Ea1 = ePA1;
  parametres.Ea2 = ePA2;
  parametres.w1 = cP1;
  parametres.w2 = cP2;
  parametres.fi1 = fip1;
  parametres.fi2 = fip2;
  parametres.t01 = t0p1;
  parametres.t02 = t0p2;
  parametres.sIgma1 = sI1;
  parametres.sIgma2 = sI2;
  parametres.p1_loc = p1_l;
  parametres.p2_loc = p2_l;

  gsl_odeiv2_system sys = {func, jac, 5, &parametres};

  gsl_odeiv2_driver * d = 
    gsl_odeiv2_driver_alloc_y_new (&sys, gsl_odeiv2_step_rk8pd, 1e-14, 1e-14, 0.0);
  
  int i;
  double shirinaImpulsa;
  shirinaImpulsa = 6.2832 * parametres.sIgma1 / parametres.w1;
  double t = t_s, t1 = t_e - t_s;
  
  double y[5] = { 0.0, 0.0, 0.0, 0.0, 0.0 };

  for (i = 1; i <= 1; i++)
    {
      double ti = t + t1 / 1.0;

      int status = gsl_odeiv2_driver_apply (d, &t, ti, y);

      if (status != GSL_SUCCESS)
	{
	  printf ("error, return value=%d\n", status);
	  break;
	}

/* Обращаемся к временному файлу calc_resalt_temp.txt                          */
/* если его нет, создаём                                                       */
/* это делает каждый процесс самостоятельно */

FILE * tf1;

    if((tf1 = fopen("Fen_2020_Zim//calc_resalt_temp.txt","a")) == NULL)
    {
    printf("Error creation file calc_resalt_temp.txt\n");
    return 1;
    }
      fprintf (tf1, "%d %.10f %.10f  %.10e %.10e  %.10e\n", n_point_tt, p1_l, p2_l, y[0], y[1], y[2]);
      fclose(tf1);
     }

  gsl_odeiv2_driver_free (d);
  return 0;
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
*  Параметры импульса поля читаются из текстового файла task_q.txt 
*  В этот файл они помещаются построчно, каждый пораметр с новой строки. При невозможности
*  найти файл с параметрами задача запущена не будет.
*
*  Данная модификация программы предназначена для работы в составе системы для автоматического
*  построения оптимальной сетки в формате квадродерева.
*  
*  Список точек для счета берется из обновляемого файла q_tree.txt
*  Файл организован построчно.
*  В каждой строке хранятся координаты точки, её параметры размещения в квадродереве, 
*  признак посчитана/непосчитана и собственно результаты счета.
*  
*  Интегрирование для каждой точки выполняется отдельным процессом. 
*  
*  
*  Результаты рассчетов вписываются в соответствующую строку файла.                              */

int
main (int argc, char **argv)
{
    double Ea1;		/*амплитуда 1-й компоненты электрического поля в графеновых единицах*/
    double Ea2;		/*амплитуда 2-й компоненты электрического поля в графеновых единицах*/
    double nu1;		/*частота 1-й компоненты поля в графеновых единицах*/
    double nu2;		/*частота 2-й компоненты поля в графеновых единицах*/
    double fi1;		/*фазовый сдвиг 1-й компоненты поля*/
    double fi2;		/*фазовый сдвиг 2-й компоненты поля*/
    double t01;		/*временной сдвиг максимума 1-й компоненты в графеновых единицах*/
    double t02;		/*временной сдвиг максимума 2-й компоненты в графеновых единицах*/
    double sIgma1;	/*ширина гауссовой огибающей 1-й компоненты*/
    double sIgma2;	/*ширина гауссовой огибающей 2-й компоненты*/
    double t_start;  /*начало интервала интегрирования*/
    double t_end;    /*конец интервала интегрирования*/

    double w1;    // первая циклическая частота
    double w2;    // вторая циклическая частота

    FILE *task_q;
    char s[80];
    
    if (argc < 3) 
    {
      printf("no files task_q.txt or q_tree.txt!\n");
      exit(1);
    }
    
    char *filename1 = argv[1];
    
    if((task_q = fopen(filename1,"r")) == NULL)
    {
    printf("Error opening file task_q.txt\n");
    return 1;
    }
    
    fscanf(task_q, "%lf %s", &Ea1, &s);
    fscanf(task_q, "%lf %s", &Ea2, &s);
    fscanf(task_q, "%lf %s", &nu1, &s);
    fscanf(task_q, "%lf %s", &nu2, &s);
    fscanf(task_q, "%lf %s", &fi1, &s);
    fscanf(task_q, "%lf %s", &fi2, &s);
    fscanf(task_q, "%lf %s", &t01, &s);
    fscanf(task_q, "%lf %s", &t02, &s);
    fscanf(task_q, "%lf %s", &sIgma1, &s);
    fscanf(task_q, "%lf %s", &sIgma2, &s);
    fscanf(task_q, "%lf %s", &t_start, &s);
    fscanf(task_q, "%lf %s", &t_end, &s);

    // закрываем файл с параметрами задания
    fclose(task_q);

    // вычисляем циклические частоты, которые используются в формулах далее
    w1 = 6.2831853 * nu1;
    w2 = 6.2831853 * nu2;

    int myid, numprocs; // параметры для самоопределения процессов

    int list_length;    // число назначенных к счету точек
                        // определяется по результату считывания строк из файла q_tree

    double p1_l, p2_l;  // локальные координаты в импульсном пространстве

    int n_point_tt;     // номер узла, является уникальным индексом в q_tree

    // Определяем структуру, в которую будем собирать точки для счета 

    //struct list 
    //{
    //int number_for_calc; 
    //int n_point;
    //double moment_1;
    //double moment_2;
    //double function_1;
    //double function_2;
    //double function_3;
    //};

    //struct list list00[1000];

    // Формируем список временных переменных, в которые будем считывать данные 
    // из строк файла q_tree                                                   
   
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

    // printf("111111111111111111\n");

    // формируем глобальный коммуникатор
   
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    FILE *q_tree;

    char *filename2 = argv[2];

       if((q_tree = fopen(filename2,"r")) == NULL)
        {
          printf("Error opening file q_tree.txt\n");
          return 1;
        }
   
       
  char i = 0;  // счетчик для подсчета считываемых строк
  
  while (fscanf (q_tree, "%d %s %s %d %s %lf %lf %s %le %le %le", &n_point_t, first_child_t, parent_t, &g_number_t, point_id_t, &p1_t, &p2_t, calculated_t, &f1_t, &f2_t, &f3_t) != EOF)
    {
   
      if (calculated_t[0] == '0')
        {
          if (n_point_t % numprocs == myid)
          {
              ode_print (n_point_t, p1_t, p2_t, Ea1, Ea2, w1, w2, sIgma1, sIgma2, t01, t02, fi1, fi2, t_start, t_end);
          };
        };
    };

    fclose(q_tree); 
    // printf ("list_length = %d\n", list_length);

    // на этом заканчивается работа основного процесса по считыванию задания
  

  // в цикле каждый поток считывает координаты предназначенной для него точки 
  // и решает систему уравнений для неё                                       
  // результаты пишутся в стандартный поток вывода  

  // int k;                          

  //   for(k = 0; k <= list_length; k++)
  // {
  //   if (myid == k % numprocs)
  //  {
  //   n_point_tt = list00[k].n_point;
  //   p1_l = list00[k].moment_1;
  //   p2_l = list00[k].moment_2;

  //   ode_print (n_point_tt, p1_l, p2_l, Ea1, Ea2, w1, w2, sIgma1, sIgma2, t01, t02, fi1, fi2, t_start, t_end);

  // 	}
  //  }

    MPI_Finalize();
    
    return 0;
}