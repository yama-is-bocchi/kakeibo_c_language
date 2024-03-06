int tyokin_input(void);
int month_create(char *mokuhyou_date,int tyokin_all);

static void tyokin_nyuryoku( int *tyokin_all );
static void tyokin_date_input( char *tyokin_date );
static int zandaka_tyokin_only_update( int tyokin_all);
static int zandaka_tyokin_all_update( int tyokin_all,char *mokuhyou_date,int tyokin_month);
