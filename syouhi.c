/***************************************/
/*     syouhi.c                       */
/*      消費入力処理プログラム        */
/**************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "syouhi.h"

extern int zandaka_codedata_tbl;         /* 残高コードデータ対照表*/
extern int syouhi_codedata_tbl[BUFF];    /* 消費コード・データ対照表  */
extern struct SYOUHI_TBL one_syouhi_tbl; /* 消費データ表    */
extern struct ZANDAKA_TBL zandaka_tbl;   /* 残高データ表*/

/****************************/
/*     消費メイン処理      */
/***************************/
int syouhi_input(void)
{

    int ret;             /* リターンコード      */
    int syouhi;          /* 消費入力*/
    int syouhi_count;    /* 消費の回数*/
    long fptr;           /* ファイルポインタ    */
    char msg[64];        /* メッセージエリア    */
    char work[128];      /* 入力ワーク                    */
    char syouhi_date[9]; /* 消費日*/
    int loop = TRUE;     /* ループフラグ                  */
    int i;               /* インデックス*/
    int now_date;

    /*残高テーブル読み込み*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }

    /*消費入力メイン処理*/
    while (loop)
    {

        syouhi_nyuryoku(&syouhi);

        if (syouhi == NG)
        {
            return NG;
        }

        time_create(&now_date);

        sprintf(syouhi_date, "%d", now_date);

        /*メッセージセット*/
        sprintf(msg, "\n 今回の支出は %d円です。よろしいですか( Y/N )", syouhi);

        /* 入力内容確認 */
        if ((ret = kakunin_input(msg)) == OK)
        {

            syouhi_count = zandaka_tbl.syouhi_count + 1; /*消費の回数セット*/

            /* 消費データ表追加 -> OK ? */
            if ((ret = syouhi_tbl_add(&fptr, syouhi_count)) == NG)
            {
                return ret;
            }

            /* 消費コード・データ対照表更新 */
            if ((ret = syouhi_codedata_tbl_update(syouhi_count, fptr)) == NG)
            {
                return ret;
            }

            /* 消費コードデータデータ表 READ -> NG ? */
            if ((ret = syouhi_codedata_tbl_read()) == NG)
            {
                return ret;
            }

            /* 消費データ表 READ -> NG ? */
            if ((ret = syouhi_tbl_read(syouhi_count)) == NG)
            {
                return ret;
            }

            /* 消費データ表 update -> NG ? */
            if ((ret = syouhi_tbl_update(syouhi, syouhi_date, syouhi_count)) == NG)
            {
                return ret;
            }

            /* 残高データ表 update -> NG ? */
            if ((ret = zandaka_syouhi_update(syouhi_count, syouhi)) == NG)
            {
                return ret;
            }

            printf("\n %4.4s-%2.2s-%2.2s      %d円の支出です", (syouhi_date + 0), (syouhi_date + 4), (syouhi_date + 6), syouhi * -1);

            /*メッセージセット*/
            sprintf(msg, "\n 入力を続けますか( Y/N )");
            if ((ret = kakunin_input(msg)) == NG)
            {
                return ret;
            }
            continue;
        }
        else

            /*メッセージセット*/
            sprintf(msg, "\n 入力を続けますか( Y/N )");
        if ((ret = kakunin_input(msg)) == NG)
        {
            return ret;
        }
    }

    return ret;
}

/*************************/
/*    支出入力処理      */
/************************/

static void syouhi_nyuryoku(int *syouhi)
{
    int mainasu;
    int loop = TRUE; /* ループフラグ */
    char work[128];  /* 入力ワーク   */

    while (loop)
    {
        printf("\n 支出を入力してくださいE (終了)");
        printf("\n ? ");

        /* 会員コード入力 */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *syouhi = NG;
            return;
        }

        /* ニューメリック・チェック -> 数値以外 ? */
        if (strspn(work, "1234567890") < strlen(work))
        {
            printf("\n 数値以外が入力されました");
            continue;
        }
                /* 桁数チェック */
        if (strlen(work) > 9)
        {
            printf("\n 桁数オーバーです");
            continue;
        }

        mainasu = atoi(work);

        *syouhi = mainasu * -1;

        break;
    }
    return;
}

/***********************/
/*   消費日入力処理   */
/**********************/

static void syouhi_date_input(char *syouhi_date)
{
    int loop = TRUE; /* ループフラグ  */
    int chk_date;    /* 日付数値      */
    char conv[3];    /* 数値変換用    */
    char work[128];  /* 入力ワーク    */

    while (loop)
    {
        printf("\n 日付を入力してください( YYYYMMDD )");
        printf("\n ? ");

        /* 日付入力 */
        work[0] = '\0';
        scanf("%s", work);

        /* 入力桁数チェック -> 8以外 ? */
        if (strlen(work) != 8)
        {
            printf("\n 入力ミスです");
            continue;
        }

        /* ニューメリック・チェック -> 数値以外 ? */
        if (strspn(work, "1234567890") < strlen(work))
        {
            printf("\n 数値以外が入力されました");
            continue;
        }

        /* 月チェック */
        conv[0] = work[4];
        conv[1] = work[5];
        conv[2] = '\0';

        chk_date = atoi(conv);
        if (chk_date > 12 || chk_date < 1)
        {
            printf("\n 日付( 月 )入力エラーです");
            continue;
        }

        /* 日チェック */
        conv[0] = work[6];
        conv[1] = work[7];
        conv[2] = '\0';
        chk_date = atoi(conv);
        if (chk_date > 31 || chk_date < 1)
        {
            printf("\n 日付( 日 )入力エラーです");
            continue;
        }

        break;
    }

    /* 入力データ セット */
    strcpy(syouhi_date, work);
    return;
}

/***************************************/
/*       消費テーブル作成              */
/**************************************/
static int syouhi_tbl_add(long *fptr, int syouhi_count)
{

    int ret;                       /* リターンコード         */
    FILE *fp;                      /* ファイルポインタ       */
    char *fname = SYOUHI_TBL_NAME; /* 消費データ表ファイル   */

    /* 消費データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n 消費データ表ファイル OPEN エラー");
        return NG;
    }

    /* 消費データ表ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n 消費データ表ファイル SEEK エラー");
        /* 消費データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* ファイルポインタ取得 */
    *fptr = ftell(fp);

    /* 消費データ表クリア */
    one_syouhi_tbl = init_one_syouhi_tbl();

    /* 消費回数セット */
    one_syouhi_tbl.count = syouhi_count;

    /* 消費データ表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                      1, fp)) != 1)
    {
        printf("\n 消費データ表ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 消費データ表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************/
/*      消費コードデータアップデート処理       */
/***********************************************/
static int syouhi_codedata_tbl_update(int syouhi_count, long fptr)
{

    int ret;                                /* リターンコード                */
    FILE *fp;                               /* ファイルポインタ              */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n コード・データ対照表ファイル READ エラー");
        ret = NG;
    }
    else
    {
        /* 該当データポインタセット */
        syouhi_codedata_tbl[syouhi_count - 1] =
            (int)((fptr / sizeof(struct KYUUYO_TBL)) + 1);

        /* ファイルポインタを先頭に SEEK -> OK でない ? */
        if ((ret = fseek(fp, 0L, SEEK_SET)) != OK)
        {
            printf("\n コード・データ対照表ファイル SEEK エラー");

            /* コード・データ対照表ファイル CLOSE */
            fclose(fp);
            return NG;
        }

        /* コード・データ対照表ファイル WRITE -> 1以外 ? */
        if ((ret = fwrite((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1,
                          fp)) != 1)
        {
            printf("\n コード・データ対照表ファイル WRITE エラー");
            ret = NG;
        }
        else
        {
            ret = OK;
        }
    }

    /* コード・データ対照表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************/
/*      消費コードデータ読み取り処理       */
/***********************************************/

int syouhi_codedata_tbl_read(void)
{

    int ret;
    FILE *fp;                               /* ファイルポインタ             */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 消費コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n 消費コード・データ対照表ファイル READ エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* コード・データ対照表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/**************************************************/
/*           消費テーブルアップデート処理         */
/*************************************************/
static int syouhi_tbl_update(int syouhi, char *syouhi_date, int syouhi_count)
{
    int ret;                       /* リターンコード         */
    long fptr;                     /* 消費データポインタ     */
    FILE *fp;                      /* ファイルポインタ       */
    char *fname = SYOUHI_TBL_NAME; /* 消費データ表ファイル   */

    one_syouhi_tbl.syouhi = syouhi;

    one_syouhi_tbl.count = syouhi_count;

    strcpy(one_syouhi_tbl.syouhi_date, syouhi_date);

    /* 消費データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n 消費データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタ セット */
    fptr = (syouhi_codedata_tbl[syouhi_count - 1] - 1) *
           sizeof(struct SYOUHI_TBL);

    /* 消費データ表ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 消費データ表ファイル SEEK エラー");

        /* 消費データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 消費データ表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                      1, fp)) != 1)
    {
        printf("\n 消費データ表ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 消費データ表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************************/
/*    残高テーブル消費回数,全体貯金額アップデート処理     */
/**********************************************************/
static int zandaka_syouhi_update(int syouhi_count, int syouhi)
{

    int ret;                        /* リターンコード         */
    long fptr;                      /* 残高データポインタ     */
    FILE *fp;                       /* ファイルポインタ       */
    char *fname = ZANDAKA_TBL_NAME; /* 残高データ表ファイル   */

    zandaka_tbl.syouhi_count = syouhi_count;

    zandaka_tbl.syouhi_all = zandaka_tbl.syouhi_all + (syouhi * -1);

    zandaka_tbl.zandaka = zandaka_tbl.zandaka + syouhi;

    /* 残高データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n 残高データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタ セット */
    fptr =
        sizeof(struct ZANDAKA_TBL);

    /* 残高データ表ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 残高データ表ファイル SEEK エラー");

        /* 残高データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 残高データ表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                      1, fp)) != 1)
    {
        printf("\n 残高データ表ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 残高データ表ファイル CLOSE */
    fclose(fp);

    return ret;
}
