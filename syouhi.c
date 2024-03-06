/***************************************/
/*     syouhi.c                       */
/*      ������͏����v���O����        */
/**************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "main.h"
#include "kyuuyo.h"
#include "syouhi.h"

extern int zandaka_codedata_tbl;         /* �c���R�[�h�f�[�^�Ώƕ\*/
extern int syouhi_codedata_tbl[BUFF];    /* ����R�[�h�E�f�[�^�Ώƕ\  */
extern struct SYOUHI_TBL one_syouhi_tbl; /* ����f�[�^�\    */
extern struct ZANDAKA_TBL zandaka_tbl;   /* �c���f�[�^�\*/

/****************************/
/*     ����C������      */
/***************************/
int syouhi_input(void)
{

    int ret;             /* ���^�[���R�[�h      */
    int syouhi;          /* �������*/
    int syouhi_count;    /* ����̉�*/
    long fptr;           /* �t�@�C���|�C���^    */
    char msg[64];        /* ���b�Z�[�W�G���A    */
    char work[128];      /* ���̓��[�N                    */
    char syouhi_date[9]; /* �����*/
    int loop = TRUE;     /* ���[�v�t���O                  */
    int i;               /* �C���f�b�N�X*/
    int now_date;

    /*�c���e�[�u���ǂݍ���*/
    if ((ret = zandaka_tbl_read()) == NG)
    {
        return ret;
    }

    /*������̓��C������*/
    while (loop)
    {

        syouhi_nyuryoku(&syouhi);

        if (syouhi == NG)
        {
            return NG;
        }

        time_create(&now_date);

        sprintf(syouhi_date, "%d", now_date);

        /*���b�Z�[�W�Z�b�g*/
        sprintf(msg, "\n ����̎x�o�� %d�~�ł��B��낵���ł���( Y/N )", syouhi);

        /* ���͓��e�m�F */
        if ((ret = kakunin_input(msg)) == OK)
        {

            syouhi_count = zandaka_tbl.syouhi_count + 1; /*����̉񐔃Z�b�g*/

            /* ����f�[�^�\�ǉ� -> OK ? */
            if ((ret = syouhi_tbl_add(&fptr, syouhi_count)) == NG)
            {
                return ret;
            }

            /* ����R�[�h�E�f�[�^�Ώƕ\�X�V */
            if ((ret = syouhi_codedata_tbl_update(syouhi_count, fptr)) == NG)
            {
                return ret;
            }

            /* ����R�[�h�f�[�^�f�[�^�\ READ -> NG ? */
            if ((ret = syouhi_codedata_tbl_read()) == NG)
            {
                return ret;
            }

            /* ����f�[�^�\ READ -> NG ? */
            if ((ret = syouhi_tbl_read(syouhi_count)) == NG)
            {
                return ret;
            }

            /* ����f�[�^�\ update -> NG ? */
            if ((ret = syouhi_tbl_update(syouhi, syouhi_date, syouhi_count)) == NG)
            {
                return ret;
            }

            /* �c���f�[�^�\ update -> NG ? */
            if ((ret = zandaka_syouhi_update(syouhi_count, syouhi)) == NG)
            {
                return ret;
            }

            printf("\n %4.4s-%2.2s-%2.2s      %d�~�̎x�o�ł�", (syouhi_date + 0), (syouhi_date + 4), (syouhi_date + 6), syouhi * -1);

            /*���b�Z�[�W�Z�b�g*/
            sprintf(msg, "\n ���͂𑱂��܂���( Y/N )");
            if ((ret = kakunin_input(msg)) == NG)
            {
                return ret;
            }
            continue;
        }
        else

            /*���b�Z�[�W�Z�b�g*/
            sprintf(msg, "\n ���͂𑱂��܂���( Y/N )");
        if ((ret = kakunin_input(msg)) == NG)
        {
            return ret;
        }
    }

    return ret;
}

/*************************/
/*    �x�o���͏���      */
/************************/

static void syouhi_nyuryoku(int *syouhi)
{
    int mainasu;
    int loop = TRUE; /* ���[�v�t���O */
    char work[128];  /* ���̓��[�N   */

    while (loop)
    {
        printf("\n �x�o����͂��Ă�������E (�I��)");
        printf("\n ? ");

        /* ����R�[�h���� */
        work[0] = '\0';
        scanf("%s", work);

        if (strcmp(work, "E") == 0 || strcmp(work, "e") == 0)
        {
            *syouhi = NG;
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

        mainasu = atoi(work);

        *syouhi = mainasu * -1;

        break;
    }
    return;
}

/***********************/
/*   ��������͏���   */
/**********************/

static void syouhi_date_input(char *syouhi_date)
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
    strcpy(syouhi_date, work);
    return;
}

/***************************************/
/*       ����e�[�u���쐬              */
/**************************************/
static int syouhi_tbl_add(long *fptr, int syouhi_count)
{

    int ret;                       /* ���^�[���R�[�h         */
    FILE *fp;                      /* �t�@�C���|�C���^       */
    char *fname = SYOUHI_TBL_NAME; /* ����f�[�^�\�t�@�C��   */

    /* ����f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n ����f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* ����f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n ����f�[�^�\�t�@�C�� SEEK �G���[");
        /* ����f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* �t�@�C���|�C���^�擾 */
    *fptr = ftell(fp);

    /* ����f�[�^�\�N���A */
    one_syouhi_tbl = init_one_syouhi_tbl();

    /* ����񐔃Z�b�g */
    one_syouhi_tbl.count = syouhi_count;

    /* ����f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                      1, fp)) != 1)
    {
        printf("\n ����f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* ����f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************/
/*      ����R�[�h�f�[�^�A�b�v�f�[�g����       */
/***********************************************/
static int syouhi_codedata_tbl_update(int syouhi_count, long fptr)
{

    int ret;                                /* ���^�[���R�[�h                */
    FILE *fp;                               /* �t�@�C���|�C���^              */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
        ret = NG;
    }
    else
    {
        /* �Y���f�[�^�|�C���^�Z�b�g */
        syouhi_codedata_tbl[syouhi_count - 1] =
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
        if ((ret = fwrite((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1,
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

/***********************************************/
/*      ����R�[�h�f�[�^�ǂݎ�菈��       */
/***********************************************/

int syouhi_codedata_tbl_read(void)
{

    int ret;
    FILE *fp;                               /* �t�@�C���|�C���^             */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n ����R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n ����R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
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

/**************************************************/
/*           ����e�[�u���A�b�v�f�[�g����         */
/*************************************************/
static int syouhi_tbl_update(int syouhi, char *syouhi_date, int syouhi_count)
{
    int ret;                       /* ���^�[���R�[�h         */
    long fptr;                     /* ����f�[�^�|�C���^     */
    FILE *fp;                      /* �t�@�C���|�C���^       */
    char *fname = SYOUHI_TBL_NAME; /* ����f�[�^�\�t�@�C��   */

    one_syouhi_tbl.syouhi = syouhi;

    one_syouhi_tbl.count = syouhi_count;

    strcpy(one_syouhi_tbl.syouhi_date, syouhi_date);

    /* ����f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n ����f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^ �Z�b�g */
    fptr = (syouhi_codedata_tbl[syouhi_count - 1] - 1) *
           sizeof(struct SYOUHI_TBL);

    /* ����f�[�^�\�t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n ����f�[�^�\�t�@�C�� SEEK �G���[");

        /* ����f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* ����f�[�^�\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                      1, fp)) != 1)
    {
        printf("\n ����f�[�^�\�t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* ����f�[�^�\�t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/***********************************************************/
/*    �c���e�[�u�������,�S�̒����z�A�b�v�f�[�g����     */
/**********************************************************/
static int zandaka_syouhi_update(int syouhi_count, int syouhi)
{

    int ret;                        /* ���^�[���R�[�h         */
    long fptr;                      /* �c���f�[�^�|�C���^     */
    FILE *fp;                       /* �t�@�C���|�C���^       */
    char *fname = ZANDAKA_TBL_NAME; /* �c���f�[�^�\�t�@�C��   */

    zandaka_tbl.syouhi_count = syouhi_count;

    zandaka_tbl.syouhi_all = zandaka_tbl.syouhi_all + (syouhi * -1);

    zandaka_tbl.zandaka = zandaka_tbl.zandaka + syouhi;

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
