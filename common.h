/************************************/
/*  common.h                        */
/*************************************/

#define BUFF   9999
#define SYOUHI_CODEDATA_TBL_NAME   "syouhi_codedata.tbl"
#define KYUUYO_CODEDATA_TBL_NAME   "kyuuyo_codedata.tbl"
#define ZANDAKA_CODEDATA_TBL_NAME  "zandaka_codedata.tbl"
#define SYOUHI_TBL_NAME    "syouhi.tbl"
#define KYUUYO_TBL_NAME    "kyuuyo.tbl"
#define ZANDAKA_TBL_NAME    "zandaka.tbl"


#define  TRUE          1
#define  FALSE         0
#define  OK            0
#define  CANCEL        1
#define  NG           -1


struct SYOUHI_TBL {
    int  count;                  /*‰½‰ñ–Ú‚ÌÁ”ï‚©*/
    char syouhi_date[ 9 ];      /*x•¥“ú•t*/
    int  syouhi;                /*Á”ïŠz*/
};

struct KYUUYO_TBL
{
    int kyuuyo;                 /*‹‹—¿*/
    char kyuuyo_date[9];        /*‹‹—¿“ú*/
    int count;                  /*‰½‰ñ–Ú‚Ì‹‹—^‚©*/
};



struct  ZANDAKA_TBL {
    int  ret;                   /*–Ú•W‚ğİ’è‚µ‚½‚©OK=İ’è‚µ‚½NG=İ’è‚µ‚Ä‚È‚¢*/
    int  tyokin_ret;           /*“ú•tİ’è‚µ‚½‚©OK=İ’è‚µ‚½NG=İ’è‚µ‚Ä‚È‚¢*/
    int  tyokin_all;           /*–Ú•W’™‹àŠz*/
    int  tyokin_month;         /*Œ•Ê–Ú•WŠz*/
    int  zandaka;              /*c‚*/
    char mokuhyou_date[9];     /*–Ú•W’B¬“ú*/
    int  syouhi_all;           /*‘S‘Ì‚ÌÁ”ïŠz*/
    int  kyuuyo_all;           /*‘S‘Ì‚Ì‹‹—^Šz*/
    int  kyuuyo_count;         /*‰½‰ñ–Ú‚Ì‹‹—^‚©*/
    int  syouhi_count;         /*‰½‰ñ–Ú‚ÌÁ”ï‚©*/
};
