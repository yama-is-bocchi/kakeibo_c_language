/*************************************************/
/*          syouhi.h                            */
/*          消費入力処理ヘッダファイル          */
/*                                              */
/************************************************/

int syouhi_input(void);
int syouhi_codedata_tbl_read( void );

static void syouhi_nyuryoku( int *syouhi );
static void syouhi_date_input( char *syouhi_date );
static int syouhi_tbl_add( long *fptr, int syouhi_count );
static int syouhi_codedata_tbl_update( int syouhi_count, long fptr );
static int syouhi_tbl_update( int syouhi,char *syouhi_date , int syouhi_count);
static int zandaka_syouhi_update( int syouhi_count,int syouhi );
