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
    int  count;                  /*����ڂ̏��*/
    char syouhi_date[ 9 ];      /*�x�����t*/
    int  syouhi;                /*����z*/
};

struct KYUUYO_TBL
{
    int kyuuyo;                 /*����*/
    char kyuuyo_date[9];        /*������*/
    int count;                  /*����ڂ̋��^��*/
};



struct  ZANDAKA_TBL {
    int  ret;                   /*�ڕW��ݒ肵����OK=�ݒ肵��NG=�ݒ肵�ĂȂ�*/
    int  tyokin_ret;           /*���t�ݒ肵����OK=�ݒ肵��NG=�ݒ肵�ĂȂ�*/
    int  tyokin_all;           /*�ڕW�����z*/
    int  tyokin_month;         /*���ʖڕW�z*/
    int  zandaka;              /*�c��*/
    char mokuhyou_date[9];     /*�ڕW�B����*/
    int  syouhi_all;           /*�S�̂̏���z*/
    int  kyuuyo_all;           /*�S�̂̋��^�z*/
    int  kyuuyo_count;         /*����ڂ̋��^��*/
    int  syouhi_count;         /*����ڂ̏��*/
};
