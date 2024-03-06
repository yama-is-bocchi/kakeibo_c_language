/************************************/
/*  kyuuyo.c                       */
/*    ���^���͏����v���O����        */
/************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "tyokin.h"

extern int zandaka_codedata_tbl;       /* �c���R�[�h�f�[�^�Ώƕ\*/
extern int kyuuyo_codedata_tbl[BUFF];  /* ���^�R�[�h�E�f�[�^�Ώƕ\  */
extern struct ZANDAKA_TBL zandaka_tbl; /* �c���f�[�^�\*/
extern struct KYUUYO_TBL kyuuyo_tbl;   /* ���^�f�[�^�\*/

/****************************/
/*     ���^���C������      */
/***************************/

int kyuuyo_input(void)
{

    int ret;             /* ���^�[���R�[�h      */
    int kyuuyo;          /* ���^����*/
    int kyuuyo_count;    /* �����̉�*/
    long fptr;           /* �t�@�C���|�C���^    */
    char msg[64];        /* ���b�Z�[�W�G���A    */
    char work[128];      /* ���̓��[�N                    */
    char kyuuyo_date[9]; /* ������*/
    int loop = TRUE;     /* ���[�v�t���O                  */
    int i;               /* �C���f�b�N�X*/
    int now_date;

    /*�c���e�[�u���ǂݍ���*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }

    /*���^���̓��C������*/
    while (loop)
    {

        kyuuyo_nyuryoku(&kyuuyo); /*���^����*/

        if (kyuuyo == NG)
        {
            return NG;
        }

        time_create(&now_date);

        sprintf(kyuuyo_date, "%d", now_date);

        /*���b�Z�[�W�Z�b�g*/
        sprintf(msg, "\n ����̋��^�� %d�~�ł��B��낵���ł���( Y/N )", kyuuyo);

        /* ���͓��e�m�F */
        if ((ret = kakunin_input(msg)) == OK)
        {

            kyuuyo_count = zandaka_tbl.kyuuyo_count + 1; /*�����̉񐔃Z�b�g*/

            /* ���^�f�[�^�\�ǉ� -> OK ? */
            if ((ret = kyuuyo_tbl_add(&fptr, kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* ���^�R�[�h�E�f�[�^�Ώƕ\�X�V */
            if ((ret = kyuuyo_codedata_tbl_update(kyuuyo_count, fptr)) == NG)
            {
                return ret;
            }

            /* ���^�R�[�h�f�[�^�f�[�^�\ READ -> NG ? */
            if ((ret = kyuuyo_codedata_tbl_read()) == NG)
            {
                return ret;
            }

            /* ���^�f�[�^�\ READ -> NG ? */
            if ((ret = kyuuyo_tbl_read(kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* ���^�f�[�^�\ update -> NG ? */
            if ((ret = kyuuyo_tbl_update(kyuuyo, kyuuyo_date, kyuuyo_count)) == NG)
            {
                return ret;
            }

            /* �c���f�[�^�\ update -> NG ? */
            if ((ret = zandaka_kyuuyo_update(kyuuyo_count, kyuuyo)) == NG)
            {
                return ret;
            }

            printf("\n %4.4s-%2.2s-%2.2s    %d�~���U�荞�܂�܂���", (kyuuyo_date + 0), (kyuuyo_date + 4), (kyuuyo_date + 6), kyuuyo);

            /*�����ڕW�ɓ��B������*/
            if (zandaka_tbl.ret == OK)
            {
                if (zandaka_tbl.tyokin_all <= zandaka_tbl.zandaka)
                {

                    printf("\n�����ڕW�z�ɓ��B���Ă��܂�\n���߂łƂ��������܂�");
                    printf("\n�����ڕW���Đݒ肵�܂��傤!!");
                }
            }
            break;
        }
        else
        {
            /*���b�Z�[�W�Z�b�g*/
            sprintf(msg, "\n ���͂𑱂��܂���( Y/N )");
            if ((ret = kakunin_input(msg)) == NG)
            {
                return ret;
            }
        }
    }

    return ret;
}

/***********************************************/
/*      ���^�R�[�h�f�[�^�A�b�v�f�[�g����       */
/***********************************************/
static int kyuuyo_codedata_tbl_update(int kyuuyo_count, long fptr)
{

    int ret;                                /* ���^�[���R�[�h                */
    FILE *fp;                               /* �t�@�C���|�C���^              */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
        ret = NG;
    }
    else
    {
        /* �Y���f�[�^�|�C���^�Z�b�g */
        kyuuyo_codedata_tbl[kyuuyo_count - 1] =
            (int)((fptr / sizeof(struct KYUUYO_TBL)) + 1);

        /* �t�@�C���|�C���^��擪�� SEEK -> OK �łȂ� ? */
        if ((ret = fseek(fp, 0L, SEEK_SET)) != OK)
        {
            printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� SEEK �G���[");

            /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */
            fclose(fp);
            return NG;
        }

        /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE -> 1�ȊO ? */
        if ((ret = fwrite((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1,
                          fp)) != 1)
        {
            printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE �G���[");
            ret = NG;
        }
        else
        {
            ret = OK;
        }
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/*******************************************/
/*      ���^�e�[�u���A�b�v�f�[�g����       */
/*******************************************/
static int kyuuyo_tbl_update(int kyuuyo, char *kyuuyo_date, int kyuuyo_count)
{
    int ret;                       /* ���^�[���R�[�h         */
    long fptr;                     /* ���^�f�[�^�|�C���^     */
    FILE *fp;                      /* �t�@�C���|�C���^       */
    char *fname = KYUUYO_TBL_NAME; /* ���^�f�[�^�\�t�@�C��   */

    kyuuyo_tbl.kyuuyo = kyuuyo;

    kyuuyo_tbl.count = kyuuyo_count;

    strcpy(kyuuyo_tbl.kyuuyo_date, kyuuyo_date);

    /* ���^�f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^ �Z�b�g */
    fptr = (kyuuyo_codedata_tbl[kyuuyo_count - 1] - 1) *
           sizeof(struct KYUUYO_TBL);

    /* ���^�f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� SEEK �G���[");

        /* ���^�f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* ���^�f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                      1, fp)) != 1)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* ���^�f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************************/
/*    �c���e�[�u�����^��,�S�̒����z�A�b�v�f�[�g����     */
/**********************************************************/
static int zandaka_kyuuyo_update(int kyuuyo_count, int kyuuyo)
{

    int ret;                        /* ���^�[���R�[�h         */
    long fptr;                      /* �c���f�[�^�|�C���^     */
    FILE *fp;                       /* �t�@�C���|�C���^       */
    char *fname = ZANDAKA_TBL_NAME; /* �c���f�[�^�\�t�@�C��   */

    zandaka_tbl.kyuuyo_count = kyuuyo_count;

    zandaka_tbl.kyuuyo_all = zandaka_tbl.kyuuyo_all + kyuuyo;

    zandaka_tbl.zandaka = zandaka_tbl.zandaka + kyuuyo;

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

/*********************/
/*   ���������͏���   */
/*********************/

static void kyuuyo_date_input(char *kyuuyo_date)
{
    int loop = TRUE; /* ���[�v�t���O  */
    int chk_date;    /* ���t���l      */
    char conv[3];    /* ���l�ϊ��p    */
    char work[128];  /* ���̓��[�N    */

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

        break;
    }

    /* ���̓f�[�^ �Z�b�g */
    strcpy(kyuuyo_date, work);
    return;
}

/***********************************/
/*     ���^�e�[�u���쐬              */
/***********************************/
static int kyuuyo_tbl_add(long *fptr, int kyuuyo_count)
{

    int ret;                       /* ���^�[���R�[�h         */
    FILE *fp;                      /* �t�@�C���|�C���^       */
    char *fname = KYUUYO_TBL_NAME; /* ���^�f�[�^�\�t�@�C��   */

    /* ���^�f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* ���^�f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� SEEK �G���[");
        /* ���^�f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* �t�@�C���|�C���^�擾 */
    *fptr = ftell(fp);

    /* ���^�f�[�^�\�N���A */
    kyuuyo_tbl = init_kyuuyo_tbl();

    /* ���^�񐔃Z�b�g */
    kyuuyo_tbl.count = kyuuyo_count;

    /* ���^�f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                      1, fp)) != 1)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* ���^�f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/****************************************/
/*      ���^�R�[�h�f�[�^�ǂݎ�菈��       */
/****************************************/

int kyuuyo_codedata_tbl_read(void)
{

    int ret;
    FILE *fp;                               /* �t�@�C���|�C���^             */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/************************************/
/*     �������͏���                  */
/************************************/
static void kyuuyo_nyuryoku(int *kyuuyo)
{
    int loop = TRUE; /* ���[�v�t���O */
    char work[128];  /* ���̓��[�N   */

    while (loop)
    {
        printf("\n ���^����͂��Ă��������BE(�I��)");
        printf("\n ? ");

        /* ����R�[�h���� */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *kyuuyo = NG;
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
        *kyuuyo = atoi(work);

        break;
    }
    return;
}
