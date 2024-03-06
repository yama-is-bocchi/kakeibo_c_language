#include <stdio.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "zandaka.h"
#include "tyokin.h"
#include "syouhi.h"

int zandaka_codedata_tbl;
int syouhi_codedata_tbl[BUFF];
int kyuuyo_codedata_tbl[BUFF];
struct SYOUHI_TBL one_syouhi_tbl;
struct ZANDAKA_TBL zandaka_tbl;
struct KYUUYO_TBL kyuuyo_tbl;

int main(void)
{

    int now_date; /*現在時間*/
    char ima_date[9];
    int loop = TRUE;                          /* ループフラグ                  */
    char work[128];                           /* 入力ワーク                    */
    char *fname1 = SYOUHI_CODEDATA_TBL_NAME;  /* 消費コード・データ対照表ファイル  */
    char *fname2 = KYUUYO_CODEDATA_TBL_NAME;  /* 給与コード・データ対照表ファイル  */
    char *fname3 = ZANDAKA_CODEDATA_TBL_NAME; /* 残高コード・データ対照表ファイル  */
    int i;
    long fptr1;
    FILE *fp1; /* 消費ファイルポインタ              */
    FILE *fp2; /* 給与ファイルポインタ              */
    FILE *fp3; /* 残高ファイルポインタ              */

    if ((fp1 = fopen(fname1, "rb")) == NULL)
    {
        printf("\n 消費コード・データ対照表ファイルを作成しています");

        /* 消費コード・データ対照表ファイル作成 */

        syouhi_codedata_tbl_create();
    }
    else
    {

        /* コード・データ対照表ファイル CLOSE */

        fclose(fp1);
    }

    if ((fp2 = fopen(fname2, "rb")) == NULL)
    {
        printf("\n 給与コード・データ対照表ファイルを作成しています");

        /* 消費コード・データ対照表ファイル作成 */

        kyuuyo_codedata_tbl_create();
    }

    else
    {

        /* コード・データ対照表ファイル CLOSE */

        fclose(fp2);
    }

    if ((fp3 = fopen(fname3, "rb")) == NULL)
    {

        /* 消費コード・データ対照表ファイル作成 */

        zandaka_tbl_create(&fptr1);

        zandaka_tbl_create(&fptr1);
    }

    else
    {

        /* コード・データ対照表ファイル CLOSE */

        fclose(fp3);
    }

    /*初期化処理*/

    while (loop)
    {

        for (i = 0; i < BUFF; i++)
        {

            syouhi_codedata_tbl[i] = 0;

            kyuuyo_codedata_tbl[i] = 0;
        }

        zandaka_codedata_tbl = 0;

        one_syouhi_tbl = init_one_syouhi_tbl();

        zandaka_tbl = init_zandaka_tbl();

        kyuuyo_tbl = init_kyuuyo_tbl();

        time_create(&now_date);

        sprintf(ima_date, "%d", now_date);

        /*表示処理*/
        printf("\n");
        printf("\n *******************************************");
        printf("\n  貯金プログラム           今日%4.4s-%2.2s-%2.2s                ", (ima_date + 0), (ima_date + 4), (ima_date + 6));
        printf("\n *******************************************");
        printf("\n 処理を選択してください");
        printf("\n 1:支出入力");
        printf("\n 2:給与入力");
        printf("\n 3:貯金目標入力");
        printf("\n 4:残高表示");
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
        case '1': /* 入会登録     */
            syouhi_input();
            break;

        case '2': /* 給与入力 */
            kyuuyo_input();
            break;

        case '3': /* 登録削除     */
            tyokin_input();
            break;

        case '4': /*残高表示*/
            zandaka_disp();
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

/*時間計算処理*/
void time_create(int *now_date)
{
    time_t timer;     /* 時刻を取り出すための型（実際はunsigned long型） */
    struct tm *local; /* tm構造体（時刻を扱う */

    /* 年月日と時分秒保存用 */
    int year, month, day, hour, minute, second;

    timer = time(NULL);        /* 現在時刻を取得 */
    local = localtime(&timer); /* 地方時に変換 */

    /* 年月日と時分秒をtm構造体の各パラメタから変数に代入 */
    year = local->tm_year + 1900; /* 1900年からの年数が取得されるため */
    month = local->tm_mon + 1;    /* 0を1月としているため */
    day = local->tm_mday;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;

    /* 現在の日時を表示 */
    *now_date = year * 10000 + month * 100 + day;
    return;
}

/************************************/
/*    消費テーブル読み取り処理      */
/************************************/
int syouhi_tbl_read(int syouhi_count)
{
    int ret;                       /* リターンコード                 */
    FILE *fp;                      /* 消費データ表ファイルポインタ   */
    long fptr;                     /* 消費データポインタ             */
    char *fname = SYOUHI_TBL_NAME; /* 消費データ表ファイル           */

    /* 消費データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 消費データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタセット */
    fptr = (syouhi_codedata_tbl[syouhi_count - 1] - 1) *
           sizeof(struct SYOUHI_TBL);

    /* 消費データ表ファイルを対象の位置まで SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 消費データ表ファイル SEEK エラー");

        /* 消費データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 消費データ表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                     1, fp)) != 1)
    {
        printf("\n 消費データ表 READ エラー");
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

/************************************/
/*    給与テーブル読み取り処理      */
/************************************/

int kyuuyo_tbl_read(int kyuuyo_count)
{
    int ret;                       /* リターンコード                 */
    FILE *fp;                      /* 給与データ表ファイルポインタ   */
    long fptr;                     /* 給与データポインタ             */
    char *fname = KYUUYO_TBL_NAME; /* 給与データ表ファイル           */

    /* 給与データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 給与データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタセット */
    fptr = (kyuuyo_codedata_tbl[kyuuyo_count - 1] - 1) *
           sizeof(struct KYUUYO_TBL);

    /* 給与データ表ファイルを対象の位置まで SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 給与データ表ファイル SEEK エラー");

        /* 給与データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 給与データ表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                     1, fp)) != 1)
    {
        printf("\n 給与データ表 READ エラー");
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

/*********************************/
/*        残高テーブル作成         */
/*********************************/
static int zandaka_tbl_create(long *fptr1)
{

    int ret;                        /* リターンコード         */
    FILE *fp;                       /* ファイルポインタ       */
    char *fname = ZANDAKA_TBL_NAME; /* 残高ファイル   */

    /* 残高ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n 残高ファイル OPEN エラー");
        return NG;
    }

    /* 残高ファイル SEEK -> OK でない ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n 残高ファイル SEEK エラー");
        /* 残高ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* ファイルポインタ取得 */
    *fptr1 = ftell(fp);

    zandaka_tbl = init_zandaka_tbl();

    if ((ret = fwrite((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                      1, fp)) != 1)
    {
        printf("\n 残高ファイル WRITE エラー");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* 残高ファイル CLOSE */
    fclose(fp);

    return ret;
}

/************************************/
/*     残高テーブル読み込み処理処理   */
/***********************************/
int zandaka_tbl_read(void)
{

    int ret;                        /* リターンコード                 */
    FILE *fp;                       /* 残高データ表ファイルポインタ   */
    long fptr;                      /* 残高データポインタ             */
    char *fname = ZANDAKA_TBL_NAME; /* 残高データ表ファイル           */

    /* 給与データ表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 残高データ表ファイル OPEN エラー");
        return NG;
    }

    /* 該当データポインタセット */
    fptr =
        sizeof(struct ZANDAKA_TBL);

    /* 給与データ表ファイルを対象の位置まで SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n 残高データ表ファイル SEEK エラー");

        /* 給与データ表ファイル CLOSE */
        fclose(fp);
        return NG;
    }

    /* 給与データ表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                     1, fp)) != 1)
    {
        printf("\n 残高データ表 READ エラー");
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

/************************************/
/*       残高コードデータ作成         */
/***********************************/

static int zandaka_codedata_tbl_create(void)
{
    int ret;                                 /* リターンコード                */
    FILE *fp;                                /* ファイルポインタ              */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */
    int i;                                   /* インデックス                  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n 残高コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    zandaka_codedata_tbl = 0;

    /* コード・データ対照表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE エラー */
        printf("\n 残高コード・データ対照表ファイル WRITE エラー");
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

/******************************/
/*     残高コードデータ読み込み  */
/******************************/
int zandaka_codedate_tbl_read(void)
{

    int ret;
    FILE *fp;                                /* ファイルポインタ             */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n 残高コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n 残高コード・データ対照表ファイル READ エラー");
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
/* 残高コードデータアップデート処理作成 */
/***********************************/

static int zandaka_codedata_tbl_update(long fptr1)
{
    int ret;                                 /* リターンコード                */
    FILE *fp;                                /* ファイルポインタ              */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n 残高コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    /* コード・データ対照表ファイル READ -> 1以外 ? */
    if ((ret = fread((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n 残高コード・データ対照表ファイル READ エラー");
        ret = NG;
    }
    else
    {
        /* 該当データポインタセット */
        zandaka_codedata_tbl = (int)((fptr1 / sizeof(struct ZANDAKA_TBL)) + 1);

        /* ファイルポインタを先頭に SEEK -> OK でない ? */
        if ((ret = fseek(fp, 0L, SEEK_SET)) != OK)
        {
            printf("\n 残高コード・データ対照表ファイル SEEK エラー");

            /* コード・データ対照表ファイル CLOSE */
            fclose(fp);
            return NG;
        }

        /* コード・データ対照表ファイル WRITE -> 1以外 ? */
        if ((ret = fwrite((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1,
                          fp)) != 1)
        {
            printf("\n 残高コード・データ対照表ファイル WRITE エラー");
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

/************************************/
/*       消費コードデータ作成         */
/***********************************/
static int syouhi_codedata_tbl_create(void)
{
    int ret;                                /* リターンコード                */
    FILE *fp;                               /* ファイルポインタ              */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */
    int i;                                  /* インデックス                  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n 消費コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    for (i = 0; i < BUFF; i++)
        syouhi_codedata_tbl[i] = 0;

    /* コード・データ対照表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE エラー */
        printf("\n 消費コード・データ対照表ファイル WRITE エラー");
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

/*****************************/
/*     給与コードデータ作成  */
/*****************************/

static int kyuuyo_codedata_tbl_create(void)
{
    int ret;                                /* リターンコード                */
    FILE *fp;                               /* ファイルポインタ              */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* コード・データ対照表ファイル  */
    int i;                                  /* インデックス                  */

    /* コード・データ対照表ファイル OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n 給与コード・データ対照表ファイル OPEN エラー");
        return NG;
    }

    for (i = 0; i < BUFF; i++)
        kyuuyo_codedata_tbl[i] = 0;

    /* コード・データ対照表ファイル WRITE -> 1以外 ? */
    if ((ret = fwrite((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE エラー */
        printf("\n 給与コード・データ対照表ファイル WRITE エラー");
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

/***********************************/
/* 共通ルーチン                    */
/*   実行確認入力処理              */
/*                                 */
/*   パラメータ :確認メッセージ    */
/*   リターン   : 0:OK             */
/*               -1:NG             */
/***********************************/
int kakunin_input(char *msg)
{
    int ret;         /* リターンコード  */
    int loop = TRUE; /* ループフラグ    */
    char work[128];  /* 入力ワーク      */

    while (loop)
    {
        /* 確認表示 */
        printf(msg);
        printf("\n ? ");

        /* Y/N入力 */
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
        case 'Y': /* Yes */
        case 'y':
            ret = OK;
            loop = FALSE;
            break;

        case 'N': /* No */
        case 'n':
            ret = NG;
            loop = FALSE;
            break;

        default:
            printf("\n 入力ミスです");
            break;
        }
    }

    return ret;
}

/*初期化*/
struct SYOUHI_TBL init_one_syouhi_tbl(void)
{
    static struct SYOUHI_TBL tbl = {
        0, "        ", 0};
    return tbl;
}

/*初期化*/
struct KYUUYO_TBL init_kyuuyo_tbl(void)
{
    static struct KYUUYO_TBL tbl = {
        0, "        ", 0};
    return tbl;
}

/*初期化*/
struct ZANDAKA_TBL init_zandaka_tbl(void)
{
    static struct ZANDAKA_TBL tbl = {
        -1, -1, 0, 0, 0, "        ", 0, 0, 0, 0};
    return tbl;
}
