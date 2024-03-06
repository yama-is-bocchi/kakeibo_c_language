/**************************************/
/*    ���^���͏����w�b�_�t�@�C��      */
/**************************************/

int kyuuyo_input(void);
int kyuuyo_codedata_tbl_read( void );

static int kyuuyo_codedata_tbl_update( int kyuuyo_count, long fptr );
static int kyuuyo_tbl_update( int kyuuyo,char *kyuuyo_date , int kyuuyo_count);
static int zandaka_kyuuyo_update( int kyuuyo_count,int kyuuyo );
static void kyuuyo_date_input( char *kyuuyo_date );
static int kyuuyo_tbl_add( long *fptr, int kyuuyo_count );
static void kyuuyo_nyuryoku( int *kyuuyo );
