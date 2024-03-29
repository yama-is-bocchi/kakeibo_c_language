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
    int  count;                  /*何回目の消費か*/
    char syouhi_date[ 9 ];      /*支払日付*/
    int  syouhi;                /*消費額*/
};

struct KYUUYO_TBL
{
    int kyuuyo;                 /*給料*/
    char kyuuyo_date[9];        /*給料日*/
    int count;                  /*何回目の給与か*/
};



struct  ZANDAKA_TBL {
    int  ret;                   /*目標を設定したかOK=設定したNG=設定してない*/
    int  tyokin_ret;           /*日付設定したかOK=設定したNG=設定してない*/
    int  tyokin_all;           /*目標貯金額*/
    int  tyokin_month;         /*月別目標額*/
    int  zandaka;              /*残高*/
    char mokuhyou_date[9];     /*目標達成日*/
    int  syouhi_all;           /*全体の消費額*/
    int  kyuuyo_all;           /*全体の給与額*/
    int  kyuuyo_count;         /*何回目の給与か*/
    int  syouhi_count;         /*何回目の消費か*/
};
