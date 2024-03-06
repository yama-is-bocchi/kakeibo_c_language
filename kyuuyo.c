/************************************/
/*  kyuuyo.c                       */
/*    給与入力処理プログラム        */
/************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "tyokin.h"

extern int zandaka_codedata_tbl;       /* 残高コードデータ対照表*/
extern int kyuuyo_codedata_tbl[BUFF];  /* 給与コード・データ対照表  */
extern struct ZANDAKA_TBL zandaka_tbl; /* 残高データ表*/
extern struct KYUUYO_TBL kyuuyo_tbl;   /* 給与データ表*/

/****************************/
/*     給与メイン処理      */
/***************************/

int kyuuyo_input(void)
{

    int ret;             /* リターンコード      */
    int kyuuyo;          /* 給与入力*/
    int kyuuyo_count;    /* 給料の回数*/
    long fptr;           /* ファイルポインタ    */
    char msg[64];        /* メッセージエリア    */
    char work[128];      /* 入力ワーク                    */
    char kyuuyo_date[9]; /* 給料日*/
    int loop = TRUE;     /* ループフラグ                  */
    int i;               /* インデックス*/
    int now_date;

    /*残高テーブル読み込み*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }

    /*給与入力メイン処理*/
    while (loop)
    {

        kyuuyo_nyuryoku(&kyuuyo); /*給与入力*/

        if (kyuuyo == NG)
        {
            return NG;
        }

        time_create(&now_date);

        sprintf(kyuuyo_date, "%d", now_date);

        /*メッセージセット*/
        sprintf(msg, "\n 今回の給与は %d円です。よろしいですか( Y/N )", kyuuyo);

        /* 入力内容確認 */
        if ((ret = kakunin_input(msg)) == OK)
        {

            kyuuyo_count = zandaka_tbl.kyuuyo_count + 1; /*給料の回数セット*/

            /* 給与データ表追加 -> OK ? */
            if ((ret = kyuuyo_tbl_add(&fptr, kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* 給与コード・データ対照表更新 */
            if ((ret = kyuuyo_codedata_tbl_update(kyuuyo_count, fptr)) == NG)
            {
                return ret;
            }

            /* 給与コードデータデータ表 READ -> NG ? */
            if ((ret = kyuuyo_codedata_tbl_read()) == NG)
            {
                return ret;
            }

            /* 給与データ表 READ -> NG ? */
            if ((ret = kyuuyo_tbl_read(kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* 給与データ表 update -> NG ? */
            if ((ret = kyuuyo_tbl_update(kyuuyo, kyuuyo_date, kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* 残高データ表 update -> NG ? */
            if ((ret = zandaka_kyuuyo_update(kyuuyo_count, kyuuyo)) == NG)
            {
                return ret;
            }

            printf("\n %4.4s-%2.2s-%2.2s    %d円が振り込まれました", (kyuuyo_date + 0), (kyuuyo_date + 4), (kyuuyo_date + 6), kyuuyo);

            /*貯金目標に到達したか*/
            if (zandaka_tbl.ret == OK)
            {
                if (zandaka_tbl.tyokin_all <= zandaka_tbl.zandaka)
                {

                    printf("\n貯金目標額に到達しています\nおめでとうございます");
                    printf("\n貯金目標を再設定しましょう!!");
                }
            }
            break;
        }
        else
        {
            /*メッセージセット*/
            sprintf(msg, "\n 入力を続けますか( Y/N )");
            if ((ret = kakunin_input(msg)) == NG)
            {
                return ret;
            }
        }
    }

    return ret;
}

/***********************************************/
/*      給与コードデータアップデート処理       */
/***********************************************/
static int kyuuyo_codedata_tbl_update(int kyuuyo_count, long fptr)
{

    int ret;                                /* リターンコード                */
    FILE *fp;                               /* ファイルポインタ              */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n コード・データ対照表ファイル READ エラー");
        ret = NG;
    }
    else
    {
        /* 該当データポインタセット */
        kyuuyo_codedata_tbl[kyuuyo_count - 1] =
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
        if ((ret = fwrite((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1,
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

/*******************************************/
/*      給与テーブルアップデート処理       */
/*******************************************/
static int kyuuyo_tbl_update(int kyuuyo, char *kyuuyo_date, int kyuuyo_count)
{
    int ret;                       /* リターンコード         */
    long fptr;                     /* 給与データポインタ     */
    FILE *fp;                      /* ファイルポインタ       */
    char *fname = KYUUYO_TBL_NAME; /* 給与データ表ファイル   */

    kyuuyo_tbl.kyuuyo = kyuuyo;

    kyuuyo_tbl.count = kyuuyo_count;

    strcpy(kyuuyo_tbl.kyuuyo_date, kyuuyo_date);

    /* 給与データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n 給与データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタ セット */
    fptr = (kyuuyo_codedata_tbl[kyuuyo_count - 1] - 1) *
           sizeof(struct KYUUYO_TBL);

    /* 給与データ表ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 給与データ表ファイル SEEK エラー");

        /* 給与データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 給与データ表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                      1, fp)) != 1)
    {
        printf("\n 給与データ表ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 給与データ表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************************/
/*    残高テーブル給与回数,全体貯金額アップデート処理     */
/**********************************************************/
static int zandaka_kyuuyo_update(int kyuuyo_count, int kyuuyo)
{

    int ret;                        /* リターンコード         */
    long fptr;                      /* 残高データポインタ     */
    FILE *fp;                       /* ファイルポインタ       */
    char *fname = ZANDAKA_TBL_NAME; /* 残高データ表ファイル   */

    zandaka_tbl.kyuuyo_count = kyuuyo_count;

    zandaka_tbl.kyuuyo_all = zandaka_tbl.kyuuyo_all + kyuuyo;

    zandaka_tbl.zandaka = zandaka_tbl.zandaka + kyuuyo;

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

/*********************/
/*   給料日入力処理   */
/*********************/

static void kyuuyo_date_input(char *kyuuyo_date)
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
    strcpy(kyuuyo_date, work);
    return;
}

/***********************************/
/*     給与テーブル作成              */
/***********************************/
static int kyuuyo_tbl_add(long *fptr, int kyuuyo_count)
{

    int ret;                       /* リターンコード         */
    FILE *fp;                      /* ファイルポインタ       */
    char *fname = KYUUYO_TBL_NAME; /* 給与データ表ファイル   */

    /* 給与データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n 給与データ表ファイル OPEN エラー");
        return NG;
    }

    /* 給与データ表ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n 給与データ表ファイル SEEK エラー");
        /* 給与データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* ファイルポインタ取得 */
    *fptr = ftell(fp);

    /* 給与データ表クリア */
    kyuuyo_tbl = init_kyuuyo_tbl();

    /* 給与回数セット */
    kyuuyo_tbl.count = kyuuyo_count;

    /* 給与データ表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                      1, fp)) != 1)
    {
        printf("\n 給与データ表ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 給与データ表ファイル CLOSE */
    fclose(fp);

    return ret;
}

/****************************************/
/*      給与コードデータ読み取り処理       */
/****************************************/

int kyuuyo_codedata_tbl_read(void)
{

    int ret;
    FILE *fp;                               /* ファイルポインタ             */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 給与コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n 給与コード・データ対照表ファイル READ エラー");
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

/************************************/
/*     給料入力処理                  */
/************************************/
static void kyuuyo_nyuryoku(int *kyuuyo)
{
    int loop = TRUE; /* ループフラグ */
    char work[128];  /* 入力ワーク   */

    while (loop)
    {
        printf("\n 給与を入力してください。E(終了)");
        printf("\n ? ");

        /* 会員コード入力 */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *kyuuyo = NG;
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
        *kyuuyo = atoi(work);

        break;
    }
    return;
}
