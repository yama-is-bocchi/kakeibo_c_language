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

    int now_date; /*���ݎ���*/
    char ima_date[9];
    int loop = TRUE;                          /* ���[�v�t���O                  */
    char work[128];                           /* ���̓��[�N                    */
    char *fname1 = SYOUHI_CODEDATA_TBL_NAME;  /* ����R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    char *fname2 = KYUUYO_CODEDATA_TBL_NAME;  /* ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    char *fname3 = ZANDAKA_CODEDATA_TBL_NAME; /* �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    int i;
    long fptr1;
    FILE *fp1; /* ����t�@�C���|�C���^              */
    FILE *fp2; /* ���^�t�@�C���|�C���^              */
    FILE *fp3; /* �c���t�@�C���|�C���^              */

    if ((fp1 = fopen(fname1, "rb")) == NULL)
    {
        printf("\n ����R�[�h�E�f�[�^�Ώƕ\�t�@�C�����쐬���Ă��܂�");

        /* ����R�[�h�E�f�[�^�Ώƕ\�t�@�C���쐬 */

        syouhi_codedata_tbl_create();
    }
    else
    {

        /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */

        fclose(fp1);
    }

    if ((fp2 = fopen(fname2, "rb")) == NULL)
    {
        printf("\n ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C�����쐬���Ă��܂�");

        /* ����R�[�h�E�f�[�^�Ώƕ\�t�@�C���쐬 */

        kyuuyo_codedata_tbl_create();
    }

    else
    {

        /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */

        fclose(fp2);
    }

    if ((fp3 = fopen(fname3, "rb")) == NULL)
    {

        /* ����R�[�h�E�f�[�^�Ώƕ\�t�@�C���쐬 */

        zandaka_tbl_create(&fptr1);

        zandaka_tbl_create(&fptr1);
    }

    else
    {

        /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */

        fclose(fp3);
    }

    /*����������*/

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

        /*�\������*/
        printf("\n");
        printf("\n *******************************************");
        printf("\n  �����v���O����           ����%4.4s-%2.2s-%2.2s                ", (ima_date + 0), (ima_date + 4), (ima_date + 6));
        printf("\n *******************************************");
        printf("\n ������I�����Ă�������");
        printf("\n 1:�x�o����");
        printf("\n 2:���^����");
        printf("\n 3:�����ڕW����");
        printf("\n 4:�c���\��");
        printf("\n E:�I��");
        printf("\n ? ");

        /* �����敪���� */
        work[0] = '\0';
        scanf("%s", work);

        /* ���͌����`�F�b�N -> 1�ȊO ? */
        if (strlen(work) != 1)
        {
            printf("\n ���̓~�X�ł�");
            continue;
        }

        switch (work[0])
        {
        case '1': /* ����o�^     */
            syouhi_input();
            break;

        case '2': /* ���^���� */
            kyuuyo_input();
            break;

        case '3': /* �o�^�폜     */
            tyokin_input();
            break;

        case '4': /*�c���\��*/
            zandaka_disp();
            break;
        case 'e': /* �I��         */
        case 'E':
            loop = FALSE;
            break;

        default:
            printf("\n ���̓~�X�ł�");
            break;
        }
    }
    return OK;
}

/*���Ԍv�Z����*/
void time_create(int *now_date)
{
    time_t timer;     /* ���������o�����߂̌^�i���ۂ�unsigned long�^�j */
    struct tm *local; /* tm�\���́i���������� */

    /* �N�����Ǝ����b�ۑ��p */
    int year, month, day, hour, minute, second;

    timer = time(NULL);        /* ���ݎ������擾 */
    local = localtime(&timer); /* �n�����ɕϊ� */

    /* �N�����Ǝ����b��tm�\���̂̊e�p�����^����ϐ��ɑ�� */
    year = local->tm_year + 1900; /* 1900�N����̔N�����擾����邽�� */
    month = local->tm_mon + 1;    /* 0��1���Ƃ��Ă��邽�� */
    day = local->tm_mday;
    hour = local->tm_hour;
    minute = local->tm_min;
    second = local->tm_sec;

    /* ���݂̓�����\�� */
    *now_date = year * 10000 + month * 100 + day;
    return;
}

/************************************/
/*    ����e�[�u���ǂݎ�菈��      */
/************************************/
int syouhi_tbl_read(int syouhi_count)
{
    int ret;                       /* ���^�[���R�[�h                 */
    FILE *fp;                      /* ����f�[�^�\�t�@�C���|�C���^   */
    long fptr;                     /* ����f�[�^�|�C���^             */
    char *fname = SYOUHI_TBL_NAME; /* ����f�[�^�\�t�@�C��           */

    /* ����f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n ����f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^�Z�b�g */
    fptr = (syouhi_codedata_tbl[syouhi_count - 1] - 1) *
           sizeof(struct SYOUHI_TBL);

    /* ����f�[�^�\�t�@�C����Ώۂ̈ʒu�܂� SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n ����f�[�^�\�t�@�C�� SEEK �G���[");

        /* ����f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* ����f�[�^�\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)&one_syouhi_tbl, sizeof(one_syouhi_tbl),
                     1, fp)) != 1)
    {
        printf("\n ����f�[�^�\ READ �G���[");
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

/************************************/
/*    ���^�e�[�u���ǂݎ�菈��      */
/************************************/

int kyuuyo_tbl_read(int kyuuyo_count)
{
    int ret;                       /* ���^�[���R�[�h                 */
    FILE *fp;                      /* ���^�f�[�^�\�t�@�C���|�C���^   */
    long fptr;                     /* ���^�f�[�^�|�C���^             */
    char *fname = KYUUYO_TBL_NAME; /* ���^�f�[�^�\�t�@�C��           */

    /* ���^�f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^�Z�b�g */
    fptr = (kyuuyo_codedata_tbl[kyuuyo_count - 1] - 1) *
           sizeof(struct KYUUYO_TBL);

    /* ���^�f�[�^�\�t�@�C����Ώۂ̈ʒu�܂� SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n ���^�f�[�^�\�t�@�C�� SEEK �G���[");

        /* ���^�f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* ���^�f�[�^�\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)&kyuuyo_tbl, sizeof(kyuuyo_tbl),
                     1, fp)) != 1)
    {
        printf("\n ���^�f�[�^�\ READ �G���[");
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

/*********************************/
/*        �c���e�[�u���쐬         */
/*********************************/
static int zandaka_tbl_create(long *fptr1)
{

    int ret;                        /* ���^�[���R�[�h         */
    FILE *fp;                       /* �t�@�C���|�C���^       */
    char *fname = ZANDAKA_TBL_NAME; /* �c���t�@�C��   */

    /* �c���t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "a+b")) == NULL)
    {
        printf("\n �c���t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �c���t�@�C�� SEEK -> OK �łȂ� ? */
    if ((ret = fseek(fp, 0L, SEEK_END)) != OK)
    {
        printf("\n �c���t�@�C�� SEEK �G���[");
        /* �c���t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* �t�@�C���|�C���^�擾 */
    *fptr1 = ftell(fp);

    zandaka_tbl = init_zandaka_tbl();

    if ((ret = fwrite((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                      1, fp)) != 1)
    {
        printf("\n �c���t�@�C�� WRITE �G���[");
        ret = NG;
    }
    else
    {
        ret = OK;
    }

    /* �c���t�@�C�� CLOSE */
    fclose(fp);

    return ret;
}

/************************************/
/*     �c���e�[�u���ǂݍ��ݏ�������   */
/***********************************/
int zandaka_tbl_read(void)
{

    int ret;                        /* ���^�[���R�[�h                 */
    FILE *fp;                       /* �c���f�[�^�\�t�@�C���|�C���^   */
    long fptr;                      /* �c���f�[�^�|�C���^             */
    char *fname = ZANDAKA_TBL_NAME; /* �c���f�[�^�\�t�@�C��           */

    /* ���^�f�[�^�\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n �c���f�[�^�\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �Y���f�[�^�|�C���^�Z�b�g */
    fptr =
        sizeof(struct ZANDAKA_TBL);

    /* ���^�f�[�^�\�t�@�C����Ώۂ̈ʒu�܂� SEEK -> OK ? */
    if ((ret = fseek(fp, fptr, SEEK_SET)) != OK)
    {
        printf("\n �c���f�[�^�\�t�@�C�� SEEK �G���[");

        /* ���^�f�[�^�\�t�@�C�� CLOSE */
        fclose(fp);
        return NG;
    }

    /* ���^�f�[�^�\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)&zandaka_tbl, sizeof(zandaka_tbl),
                     1, fp)) != 1)
    {
        printf("\n �c���f�[�^�\ READ �G���[");
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

/************************************/
/*       �c���R�[�h�f�[�^�쐬         */
/***********************************/

static int zandaka_codedata_tbl_create(void)
{
    int ret;                                 /* ���^�[���R�[�h                */
    FILE *fp;                                /* �t�@�C���|�C���^              */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    int i;                                   /* �C���f�b�N�X                  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    zandaka_codedata_tbl = 0;

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE �G���[ */
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE �G���[");
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

/******************************/
/*     �c���R�[�h�f�[�^�ǂݍ���  */
/******************************/
int zandaka_codedate_tbl_read(void)
{

    int ret;
    FILE *fp;                                /* �t�@�C���|�C���^             */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "rb")) == NULL)
    {
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
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
/* �c���R�[�h�f�[�^�A�b�v�f�[�g�����쐬 */
/***********************************/

static int zandaka_codedata_tbl_update(long fptr1)
{
    int ret;                                 /* ���^�[���R�[�h                */
    FILE *fp;                                /* �t�@�C���|�C���^              */
    char *fname = ZANDAKA_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "r+b")) == NULL)
    {
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ -> 1�ȊO ? */
    if ((ret = fread((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1, fp)) != 1)
    {
        printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� READ �G���[");
        ret = NG;
    }
    else
    {
        /* �Y���f�[�^�|�C���^�Z�b�g */
        zandaka_codedata_tbl = (int)((fptr1 / sizeof(struct ZANDAKA_TBL)) + 1);

        /* �t�@�C���|�C���^��擪�� SEEK -> OK �łȂ� ? */
        if ((ret = fseek(fp, 0L, SEEK_SET)) != OK)
        {
            printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� SEEK �G���[");

            /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� CLOSE */
            fclose(fp);
            return NG;
        }

        /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE -> 1�ȊO ? */
        if ((ret = fwrite((char *)zandaka_codedata_tbl, sizeof(zandaka_codedata_tbl), 1,
                          fp)) != 1)
        {
            printf("\n �c���R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE �G���[");
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

/************************************/
/*       ����R�[�h�f�[�^�쐬         */
/***********************************/
static int syouhi_codedata_tbl_create(void)
{
    int ret;                                /* ���^�[���R�[�h                */
    FILE *fp;                               /* �t�@�C���|�C���^              */
    char *fname = SYOUHI_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    int i;                                  /* �C���f�b�N�X                  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n ����R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    for (i = 0; i < BUFF; i++)
        syouhi_codedata_tbl[i] = 0;

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)syouhi_codedata_tbl, sizeof(syouhi_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE �G���[ */
        printf("\n ����R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE �G���[");
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

/*****************************/
/*     ���^�R�[�h�f�[�^�쐬  */
/*****************************/

static int kyuuyo_codedata_tbl_create(void)
{
    int ret;                                /* ���^�[���R�[�h                */
    FILE *fp;                               /* �t�@�C���|�C���^              */
    char *fname = KYUUYO_CODEDATA_TBL_NAME; /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C��  */
    int i;                                  /* �C���f�b�N�X                  */

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN -> NULL ? */
    if ((fp = fopen(fname, "w+b")) == NULL)
    {
        printf("\n ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C�� OPEN �G���[");
        return NG;
    }

    for (i = 0; i < BUFF; i++)
        kyuuyo_codedata_tbl[i] = 0;

    /* �R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE -> 1�ȊO ? */
    if ((ret = fwrite((char *)kyuuyo_codedata_tbl, sizeof(kyuuyo_codedata_tbl), 1, fp)) != 1)
    {
        /* WRITE �G���[ */
        printf("\n ���^�R�[�h�E�f�[�^�Ώƕ\�t�@�C�� WRITE �G���[");
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

/***********************************/
/* ���ʃ��[�`��                    */
/*   ���s�m�F���͏���              */
/*                                 */
/*   �p�����[�^ :�m�F���b�Z�[�W    */
/*   ���^�[��   : 0:OK             */
/*               -1:NG             */
/***********************************/
int kakunin_input(char *msg)
{
    int ret;         /* ���^�[���R�[�h  */
    int loop = TRUE; /* ���[�v�t���O    */
    char work[128];  /* ���̓��[�N      */

    while (loop)
    {
        /* �m�F�\�� */
        printf(msg);
        printf("\n ? ");

        /* Y/N���� */
        work[0] = '\0';
        scanf("%s", work);

        /* ���͌����`�F�b�N -> 1�ȊO ? */
        if (strlen(work) != 1)
        {
            printf("\n ���̓~�X�ł�");
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
            printf("\n ���̓~�X�ł�");
            break;
        }
    }

    return ret;
}

/*������*/
struct SYOUHI_TBL init_one_syouhi_tbl(void)
{
    static struct SYOUHI_TBL tbl = {
        0, "        ", 0};
    return tbl;
}

/*������*/
struct KYUUYO_TBL init_kyuuyo_tbl(void)
{
    static struct KYUUYO_TBL tbl = {
        0, "        ", 0};
    return tbl;
}

/*������*/
struct ZANDAKA_TBL init_zandaka_tbl(void)
{
    static struct ZANDAKA_TBL tbl = {
        -1, -1, 0, 0, 0, "        ", 0, 0, 0, 0};
    return tbl;
}
