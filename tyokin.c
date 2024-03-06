/***************************************/
/*      tyokin.c                       */
/*      消費入力処理プログラム        */
/**************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "tyokin.h"

extern int zandaka_codedata_tbl;       /* 残高コードデータ対照表*/
extern struct ZANDAKA_TBL zandaka_tbl; /* 残高データ表*/

/*********************************/
/*       貯金目標メイン処理      */
/*********************************/
int tyokin_input(void)
{
    int ret;               /* リターンコード      */
    int now_date;          /* 現在時刻int型*/
    char ima_date[9];      /* 現在時刻char型*/
    int tyokin_all;        /* 貯金目標額*/
    int tyokin_month;      /* 月別目標額*/
    long fptr;             /* ファイルポインタ    */
    char msg[64];          /* メッセージエリア    */
    char work[128];        /* 入力ワーク                    */
    char mokuhyou_date[9]; /* 目標達成日付*/
    int want_time;         /* 目標達成に必要な日数*/
    int loop = TRUE;       /* ループフラグ                  */
    int i;                 /* インデックス*/
    int hantei;            /*判定用OK,NG*/
    int t;                 /*保存用*/

    /*残高テーブル読み込み*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }
    /*貯金目標入力メイン処理*/
    while (loop)
    {

        tyokin_nyuryoku(&tyokin_all);
        if (tyokin_all == NG)
        {
            return NG;
        }

        /*すでに目標に達しているか*/
        if (tyokin_all < zandaka_tbl.zandaka)
        {

            printf("\n 残高が既に目標に達成しています\n再設定してください");

            continue;
        }
        /*メッセージセット*/
        sprintf(msg, "\n 達成日付を設定しますか( Y/N )");

        /* 入力内容確認 */
        if ((ret = kakunin_input(msg)) == OK)
        {

            tyokin_date_input(mokuhyou_date);
            zandaka_tbl.tyokin_ret = OK;
        }
        else if (ret == NG)
        {

            zandaka_tbl.tyokin_ret = NG;
        }

        if (ret == OK)
        {

            /*メッセージセット*/
            t = tyokin_all;
            sprintf(msg, "\n 今回の目標貯金額は %d で,\n目標達成日付は%4.4s-%2.2s-%2.2sです。よろしいですか( Y/N )", tyokin_all, (mokuhyou_date + 0), (mokuhyou_date + 4), (mokuhyou_date + 6));

            /* 入力内容確認 */
            if ((ret = kakunin_input(msg)) == NG)
            {

                sprintf(msg, "\n 入力を続けますか( Y/N )");

                /* 入力内容確認 */
                if ((ret = kakunin_input(msg)) == NG)
                {

                    return ret;
                }

                continue;
            }
            hantei = OK;
        }
        else if (ret == NG)
        {
            t = tyokin_all;
            sprintf(msg, "\n 今回の目標貯金額は %d円です。よろしいですか( Y/N )", tyokin_all);

            /* 入力内容確認 */
            if ((ret = kakunin_input(msg)) == NG)
            {

                sprintf(msg, "\n 入力を続けますか( Y/N )");

                /* 入力内容確認 */
                if ((ret = kakunin_input(msg)) == NG)
                {

                    return ret;
                }

                continue;
            }
            hantei = NG;
        }
        break;
    }

    if (hantei == NG)
    {
        /*貯金額のみアップデート*/
        tyokin_all = t;
        zandaka_tbl.ret = OK;
        if ((ret = zandaka_tyokin_only_update(tyokin_all)) == NG)
        {
            return ret;
        }
    }
    else if (hantei == OK)
    {
        tyokin_all = t;
        tyokin_month = month_create(mokuhyou_date, tyokin_all);
        /*貯金額日付月別アップデート*/
        zandaka_tbl.ret = OK;
        if ((ret = zandaka_tyokin_all_update(tyokin_all, mokuhyou_date, tyokin_month)) == NG)
        {
            return ret;
        }
    }

    printf("\n現在の貯金目標額は%dです\n頑張りましょう!!!", tyokin_all);

    return ret;
}

/***************************************/
/*     月別目標計算処理                */
/*  必ず残高テーブルを読み込むこと     */
/**************************************/
int month_create(char *mokuhyou_date, int tyokin_all)
{
    int year;
    int mokuhyou_int; /* int型目標日付*/
    int now_date;
    int want_time;
    int tyokin_month;

    time_create(&now_date);

    mokuhyou_int = atoi(mokuhyou_date);
    /*年をセット*/
    now_date = now_date / 100;

    mokuhyou_int = mokuhyou_int / 100;

    want_time = mokuhyou_int / 100 - now_date / 100;

    year = mokuhyou_int / 100;

    mokuhyou_int = mokuhyou_int - year * 100;

    year = now_date / 100;

    now_date = now_date - year * 100;

    want_time = (mokuhyou_int + (want_time * 12)) - now_date;

    tyokin_month = (tyokin_all - zandaka_tbl.zandaka) / want_time;

    return tyokin_month;
}

/*************************/
/*    目標入力処理      */
/************************/

static void tyokin_nyuryoku(int *tyokin_all)
{
    int loop = TRUE; /* ループフラグ */
    char work[128];  /* 入力ワーク   */

    while (loop)
    {
        printf("\n 貯金目標額を入力してくださいE(終了)");
        printf("\n ? ");

        /* 会員コード入力 */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *tyokin_all = NG;
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

        *tyokin_all = atoi(work);

        break;
    }
    return;
}

/***********************/
/*   目標日付入力処理   */
/**********************/

static void tyokin_date_input(char *mokuhyou_date)
{
    int loop = TRUE; /* ループフラグ  */
    int ret;         /*リターンコード*/
    int chk_date;    /* 日付数値      */
    char msg[64];    /* メッセージエリア    */
    char conv[3];    /* 数値変換用    */
    char work[128];  /* 入力ワーク    */
    char kakunin[9]; /*確認用*/
    int int_kakunin;
    int now_date;

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
        /* 入力データ セット */
        time_create(&now_date);

        int_kakunin = atoi(work);

        if (int_kakunin - now_date <= 0)
        {

            printf("\n 目標日付が現在時刻より昔、もしくは現在に設定されています");
            continue;
        }

        int_kakunin = int_kakunin / 100;
        now_date = now_date / 100;

        if (int_kakunin - now_date == 0)
        {

            printf("\n目標の日付が同月に設定されています");
            continue;
        }

        strcpy(kakunin, work);

        /*メッセージセット*/
        sprintf(msg, "\n %4.4s-%2.2s-%2.2sでよろしいですか( Y/N )", (kakunin + 0), (kakunin + 4), (kakunin + 6));

        /* 入力内容確認 */
        if ((ret = kakunin_input(msg)) == NG)
        {

            continue;
        }

        break;
    }

    /* 入力データ セット */
    strcpy(mokuhyou_date, work);
    return;
}

/**************************************************/
/*          1残高テーブルアップデート処理         */
/*************************************************/
static int zandaka_tyokin_only_update(int tyokin_all)
{
    int ret;                        /* リターンコード         */
    long fptr;                      /* 残高データポインタ     */
    FILE *fp;                       /* ファイルポインタ       */
    char *fname = ZANDAKA_TBL_NAME; /* 残高データ表ファイル   */

    zandaka_tbl.tyokin_all = tyokin_all;

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

/**************************************************/
/*          2残高テーブルアップデート処理         */
/*************************************************/
static int zandaka_tyokin_all_update(int tyokin_all, char *mokuhyou_date, int tyokin_month)
{
    int ret;                        /* リターンコード         */
    long fptr;                      /* 残高データポインタ     */
    FILE *fp;                       /* ファイルポインタ       */
    char *fname = ZANDAKA_TBL_NAME; /* 残高データ表ファイル   */

    zandaka_tbl.tyokin_all = tyokin_all;
    strcpy(zandaka_tbl.mokuhyou_date, mokuhyou_date);
    zandaka_tbl.tyokin_month = tyokin_month;

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
