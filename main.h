/************************************************/
/*  main.h                                      */
/*                                         */
/************************************************/
void time_create(int *now_date);
int syouhi_tbl_read( int syouhi_count );
int kyuuyo_tbl_read( int kyuuyo_count );
int zandaka_tbl_read(void);
int zandaka_codedate_tbl_read(void);
int kakunin_input( char *msg );

static int zandaka_tbl_create( long *fptr1 );
static int zandaka_codedata_tbl_create( void );
static int zandaka_codedata_tbl_update(  long fptr1 );
static int syouhi_codedata_tbl_create( void );
static int kyuuyo_codedata_tbl_create( void );
struct SYOUHI_TBL init_one_syouhi_tbl( void );
struct KYUUYO_TBL init_kyuuyo_tbl( void );
struct ZANDAKA_TBL init_zandaka_tbl( void );
