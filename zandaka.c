#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "zandaka.h"
#include "syouhi.h"
#include "tyokin.h"

extern int zandaka_codedata_tbl;
extern int syouhi_codedata_tbl[BUFF];
extern int kyuuyo_codedata_tbl[BUFF];
extern struct SYOUHI_TBL one_syouhi_tbl;
extern struct ZANDAKA_TBL zandaka_tbl;
extern struct KYUUYO_TBL kyuuyo_tbl;

/****************************/
/*    残高表示メイン処理    */
/****************************/
int zandaka_disp(void)
{

    int ret;          /*リターンコード*/
    int now_date;     /*現在時間*/
    char ima_date[9]; /*現在時刻文字型*/
    int loop = TRUE;  /* ループフラグ                  */
    char work[128];   /* 入力ワーク                    */

    /*残高テーブル読み込み*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }

    while (loop)
    {

        printf("\n\n 現在残高     %d円\n", zandaka_tbl.zandaka);
        printf("\n 1:給料の総額一覧");
        printf("\n 2:支出の総額一覧");
        printf("\n 3:貯金目標表示");
        printf("\n E:終了");
        printf("\n ? ");

        /* 処理区分入力 */
        work[0] = '\0';
        scanf("%s", work);

        /* 入力桁数チェック -> 1以外 ? */
        if (strlen(work) != 1)
        {
            printf("\n 入力ミスです");
            continue;
        }

        switch (work[0])
        {
        case '1': /* 給与表示    */
            kyuuyo_disp();
            break;

        case '2': /* 消費表示 */
            syouhi_disp();
            break;

        case '3': /* 貯金表示 */
            tyokin_disp();
            break;

        case 'e': /* 終了         */
        case 'E':
            loop = FALSE;
            break;

        default:
            printf("\n 入力ミスです");
            break;
        }
    }
    return OK;
}
/*************************/
/*     給与表示処理      */
/*************************/
static int kyuuyo_disp(void)
{

    int ret;    /*リターンコード*/
    int i;      /*インデックス*/
    int hantei; /*10回カウントの判定*/

    if (zandaka_tbl.kyuuyo_count <= 0)
    {

        printf("\n給与が入力されていません");

        return NG;
    }
    /*給与コードデータ読み込み*/
    if ((ret = kyuuyo_codedata_tbl_read()) == NG)
    {
        return ret;
    }

    printf("\n現在の給与総額は%d円です\n", zandaka_tbl.kyuuyo_all);

    if (zandaka_tbl.kyuuyo_count < 30)
    {

        hantei = zandaka_tbl.kyuuyo_count;
    }
    else
    {

        hantei = 30;
    }

    for (i = 0; i < hantei; i++)
    {

        /*給与コードデータ読み込み*/
        if ((ret = kyuuyo_tbl_read(zandaka_tbl.kyuuyo_count - i)) == NG)
        {
            return ret;
        }

        printf("\n%d回前  %4.4s-%2.2s-%2.2s     %d円", i + 1, (kyuuyo_tbl.kyuuyo_date + 0), (kyuuyo_tbl.kyuuyo_date + 4), (kyuuyo_tbl.kyuuyo_date + 6), kyuuyo_tbl.kyuuyo);
    }

    /* キー入力待ち */
    while (getchar() != '\n')
        ;
    printf("\n\n\n リターンキーを押してください");
    getchar();

    return OK;
}

/*************************/
/*     消費表示処理      */
/*************************/
static int syouhi_disp(void)
{

    int ret;    /*リターンコード*/
    int i;      /*インデックス*/
    int hantei; /*10回カウントの判定*/

    if (zandaka_tbl.syouhi_count <= 0)
    {

        printf("\n消費が入力されていません");

        return NG;
    }
    /*消費コードデータ読み込み*/
    if ((ret = syouhi_codedata_tbl_read()) == NG)
    {
        return ret;
    }

    printf("\n現在の消費総額は%d円です\n", zandaka_tbl.syouhi_all);

    if (zandaka_tbl.syouhi_count < 30)
    {

        hantei = zandaka_tbl.syouhi_count;
    }
    else
    {

        hantei = 30;
    }

    for (i = 0; i < hantei; i++)
    {

        /*消費コードデータ読み込み*/
        if ((ret = syouhi_tbl_read(zandaka_tbl.syouhi_count - i)) == NG)
        {
            return ret;
        }

        printf("\n%d回前  %4.4s-%2.2s-%2.2s     %d円", i + 1, (one_syouhi_tbl.syouhi_date + 0), (one_syouhi_tbl.syouhi_date + 4), (one_syouhi_tbl.syouhi_date + 6), one_syouhi_tbl.syouhi);
    }

    /* キー入力待ち */
    while (getchar() != '\n')
        ;
    printf("\n\n\n リターンキーを押してください");
    getchar();
    return OK;
}

/*************************/
/*     貯金表示処理      */
/*************************/
static int tyokin_disp(void)
{
    int tyokin_month;
    int ret;
    int now_date;
    int mokuhyou_int;

    if (zandaka_tbl.ret == NG)
    {

        printf("\n貯金目標が設定されていません");
        return NG;
    }

    if (zandaka_tbl.tyokin_ret == OK)
    {

        time_create(&now_date);

        mokuhyou_int = atoi(zandaka_tbl.mokuhyou_date);

        mokuhyou_int = mokuhyou_int - now_date;

        if (mokuhyou_int < 0)
        {

            printf("\n 貯金目標額        %d円\n 差額は             %d円", zandaka_tbl.tyokin_all, zandaka_tbl.tyokin_all - zandaka_tbl.zandaka);
            printf("\n現在日付が目標日付に達成しています\n目標日付を設定しなおしてください");

            /* キー入力待ち */
            while (getchar() != '\n')
                ;
            printf("\n\n\n リターンキーを押してください");
            getchar();
            return NG;
        }

        tyokin_month = month_create(zandaka_tbl.mokuhyou_date, zandaka_tbl.tyokin_all);

        printf("\n                  貯金目標額 = ( %d円 )\n                      差額は = ( %d円 )\n %4.4s-%2.2s-%2.2sまでの1か月の目標額は( %d円 )", zandaka_tbl.tyokin_all, zandaka_tbl.tyokin_all - zandaka_tbl.zandaka, (zandaka_tbl.mokuhyou_date + 0), (zandaka_tbl.mokuhyou_date + 4), (zandaka_tbl.mokuhyou_date + 6), tyokin_month);
    }
    else if (zandaka_tbl.tyokin_ret == NG)
    {

        printf("\n 貯金目標額        %d円\n 差額は             %d円", zandaka_tbl.tyokin_all, zandaka_tbl.tyokin_all - zandaka_tbl.zandaka);
    }
    /* キー入力待ち */
    while (getchar() != '\n')
        ;
    printf("\n\n\n リターンキーを押してください");
    getchar();
    return OK;
}
