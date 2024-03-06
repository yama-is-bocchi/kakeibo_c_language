/***************************************/
/*      tyokin.c                       */
/*      ������͏����v���O����        */
/**************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "tyokin.h"

extern int zandaka_codedata_tbl;       /* �c���R�[�h�f�[�^�Ώƕ\*/
extern struct ZANDAKA_TBL zandaka_tbl; /* �c���f�[�^�\*/

/*********************************/
/*       �����ڕW���C������      */
/*********************************/
int tyokin_input(void)
{
    int ret;               /* ���^�[���R�[�h      */
    int now_date;          /* ���ݎ���int�^*/
    char ima_date[9];      /* ���ݎ���char�^*/
    int tyokin_all;        /* �����ڕW�z*/
    int tyokin_month;      /* ���ʖڕW�z*/
    long fptr;             /* �t�@�C���|�C���^    */
    char msg[64];          /* ���b�Z�[�W�G���A    */
    char work[128];        /* ���̓��[�N                    */
    char mokuhyou_date[9]; /* �ڕW�B�����t*/
    int want_time;         /* �ڕW�B���ɕK�v�ȓ���*/
    int loop = TRUE;       /* ���[�v�t���O                  */
    int i;                 /* �C���f�b�N�X*/
    int hantei;            /*����pOK,NG*/
    int t;                 /*�ۑ��p*/

    /*�c���e�[�u���ǂݍ���*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }
    /*�����ڕW���̓��C������*/
    while (loop)
    {

        tyokin_nyuryoku(&tyokin_all);
        if (tyokin_all == NG)
        {
            return NG;
        }

        /*���łɖڕW�ɒB���Ă��邩*/
        if (tyokin_all < zandaka_tbl.zandaka)
        {

            printf("\n �c�������ɖڕW�ɒB�����Ă��܂�\n�Đݒ肵�Ă�������");

            continue;
        }
        /*���b�Z�[�W�Z�b�g*/
        sprintf(msg, "\n �B�����t��ݒ肵�܂���( Y/N )");

        /* ���͓��e�m�F */
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

            /*���b�Z�[�W�Z�b�g*/
            t = tyokin_all;
            sprintf(msg, "\n ����̖ڕW�����z�� %d ��,\n�ڕW�B�����t��%4.4s-%2.2s-%2.2s�ł��B��낵���ł���( Y/N )", tyokin_all, (mokuhyou_date + 0), (mokuhyou_date + 4), (mokuhyou_date + 6));

            /* ���͓��e�m�F */
            if ((ret = kakunin_input(msg)) == NG)
            {

                sprintf(msg, "\n ���͂𑱂��܂���( Y/N )");

                /* ���͓��e�m�F */
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
            sprintf(msg, "\n ����̖ڕW�����z�� %d�~�ł��B��낵���ł���( Y/N )", tyokin_all);

            /* ���͓��e�m�F */
            if ((ret = kakunin_input(msg)) == NG)
            {

                sprintf(msg, "\n ���͂𑱂��܂���( Y/N )");

                /* ���͓��e�m�F */
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
        /*�����z�̂݃A�b�v�f�[�g*/
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
        /*�����z���t���ʃA�b�v�f�[�g*/
        zandaka_tbl.ret = OK;
        if ((ret = zandaka_tyokin_all_update(tyokin_all, mokuhyou_date, tyokin_month)) == NG)
        {
            return ret;
        }
    }

    printf("\n���݂̒����ڕW�z��%d�ł�\n�撣��܂��傤!!!", tyokin_all);

    return ret;
}

/***************************************/
/*     ���ʖڕW�v�Z����                */
/*  �K���c���e�[�u����ǂݍ��ނ���     */
/**************************************/
int month_create(char *mokuhyou_date, int tyokin_all)
{
    int year;
    int mokuhyou_int; /* int�^�ڕW���t*/
    int now_date;
    int want_time;
    int tyokin_month;

    time_create(&now_date);

    mokuhyou_int = atoi(mokuhyou_date);
    /*�N���Z�b�g*/
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
/*    �ڕW���͏���      */
/************************/

static void tyokin_nyuryoku(int *tyokin_all)
{
    int loop = TRUE; /* ���[�v�t���O */
    char work[128];  /* ���̓��[�N   */

    while (loop)
    {
        printf("\n �����ڕW�z����͂��Ă�������E(�I��)");
        printf("\n ? ");

        /* ����R�[�h���� */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *tyokin_all = NG;
            return;
        }

        /* �j���[�����b�N�E�`�F�b�N -> ���l�ȊO ? */
        if (strspn(work, "1234567890") < strlen(work))
        {
            printf("\n ���l�ȊO�����͂���܂���");
            continue;
        }
                /* �����`�F�b�N */
        if (strlen(work) > 9)
        {
            printf("\n �����I�[�o�[�ł�");
            continue;
        }

        *tyokin_all = atoi(work);

        break;
    }
    return;
}

/***********************/
/*   �ڕW���t���͏���   */
/**********************/

static void tyokin_date_input(char *mokuhyou_date)
{
    int loop = TRUE; /* ���[�v�t���O  */
    int ret;         /*���^�[���R�[�h*/
    int chk_date;    /* ���t���l      */
    char msg[64];    /* ���b�Z�[�W�G���A    */
    char conv[3];    /* ���l�ϊ��p    */
    char work[128];  /* ���̓��[�N    */
    char kakunin[9]; /*�m�F�p*/
    int int_kakunin;
    int now_date;

    while (loop)
    {
        printf("\n ���t����͂��Ă�������( YYYYMMDD )");
        printf("\n ? ");

        /* ���t���� */
        work[0] = '\0';
        scanf("%s", work);

        /* ���͌����`�F�b�N -> 8�ȊO ? */
        if (strlen(work) != 8)
        {
            printf("\n ���̓~�X�ł�");
            continue;
        }

        /* �j���[�����b�N�E�`�F�b�N -> ���l�ȊO ? */
        if (strspn(work, "1234567890") < strlen(work))
        {
            printf("\n ���l�ȊO�����͂���܂���");
            continue;
        }

        /* ���`�F�b�N */
        conv[0] = work[4];
        conv[1] = work[5];
        conv[2] = '\0';

        chk_date = atoi(conv);
        if (chk_date > 12 || chk_date < 1)
        {
            printf("\n ���t( �� )���̓G���[�ł�");
            continue;
        }

        /* ���`�F�b�N */
        conv[0] = work[6];
        conv[1] = work[7];
        conv[2] = '\0';
        chk_date = atoi(conv);
        if (chk_date > 31 || chk_date < 1)
        {
            printf("\n ���t( �� )���̓G���[�ł�");
            continue;
        }
        /* ���̓f�[�^ �Z�b�g */
        time_create(&now_date);

        int_kakunin = atoi(work);

        if (int_kakunin - now_date <= 0)
        {

            printf("\n �ڕW���t�����ݎ������́A�������͌��݂ɐݒ肳��Ă��܂�");
            continue;
        }

        int_kakunin = int_kakunin / 100;
        now_date = now_date / 100;

        if (int_kakunin - now_date == 0)
        {

            printf("\n�ڕW�̓��t�������ɐݒ肳��Ă��܂�");
            continue;
        }

        strcpy(kakunin, work);

        /*���b�Z�[�W�Z�b�g*/
        sprintf(msg, "\n %4.4s-%2.2s-%2.2s�ł�낵���ł���( Y/N )", (kakunin + 0), (kakunin + 4), (kakunin + 6));

        /* ���͓��e�m�F */
        if ((ret = kakunin_input(msg)) == NG)
        {

            continue;
        }

        break;
    }

    /* ���̓f�[�^ �Z�b�g */
    strcpy(mokuhyou_date, work);
    return;
}

/**************************************************/
/*          1�c���e�[�u���A�b�v�f�[�g����         */
/*************************************************/
static int zandaka_tyokin_only_update(int tyokin_all)
{
    int ret;                        /* ���^�[���R�[�h         */
    long fptr;                      /* �c���f�[�^�|�C���^     */
    FILE *fp;                       /* �t�@�C���|�C���^       */
    char *fname = ZANDAKA_TBL_NAME; /* �c���f�[�^�\�t�@�C��   */

    zandaka_tbl.tyokin_all = tyokin_all;

    /* �c���f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n �c���f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^ �Z�b�g */
    fptr =
        sizeof(struct ZANDAKA_TBL);

    /* �c���f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n �c���f�[�^�\�t�@�C�� SEEK �G���[");

        /* �c���f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* �c���f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                      1, fp)) != 1)
    {
        printf("\n �c���f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* �c���f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/**************************************************/
/*          2�c���e�[�u���A�b�v�f�[�g����         */
/*************************************************/
static int zandaka_tyokin_all_update(int tyokin_all, char *mokuhyou_date, int tyokin_month)
{
    int ret;                        /* ���^�[���R�[�h         */
    long fptr;                      /* �c���f�[�^�|�C���^     */
    FILE *fp;                       /* �t�@�C���|�C���^       */
    char *fname = ZANDAKA_TBL_NAME; /* �c���f�[�^�\�t�@�C��   */

    zandaka_tbl.tyokin_all = tyokin_all;
    strcpy(zandaka_tbl.mokuhyou_date, mokuhyou_date);
    zandaka_tbl.tyokin_month = tyokin_month;

    /* �c���f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n �c���f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^ �Z�b�g */
    fptr =
        sizeof(struct ZANDAKA_TBL);

    /* �c���f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n �c���f�[�^�\�t�@�C�� SEEK �G���[");

        /* �c���f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* �c���f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                      1, fp)) != 1)
    {
        printf("\n �c���f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* �c���f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}
