#ifndef _ALG_PLUS_DEFS_H_
#define _ALG_PLUS_DEFS_H_

// ���ϴ���
enum emErrorMode
{
	EM_SUCCESS       = 0,	 // ���Գɹ�
    EM_1111          = 1,    // �����ر�ʯ���쳣���״ο���2ǰ��ǰ��ص�ͨ��
	EM_1112          = 2,    // �����ر�ʯ���쳣����һ�ο���2��1s�ڣ�ǰ��ص�ͨ��
	EM_1124          = 3,    // ������©�������ν�ѹ��������˲���
	EM_1131          = 4,    // ������ʱ�쳣��ǰ10s��-INFANT/CHILD only
	EM_1132          = 5,    // ������ʱ�쳣����10s��
	EM_2033          = 6,    // �ڲ�ͨѶ���ȶ������ݶ�����
	EM_2091          = 7,    // �����ص缫�Ӵ��쳣��̽��Խ����⣩
};

// �ο���ģʽ
enum emRefGroupMode
{
	RGM_GENERAL    = 0,	   // ͨ��
	RGM_MALE	   = 1,	   // ����
	RGM_FEMALE	   = 2,	   // ��Ů
    RGM_INFANT     = 3,    // Ӥ�׶�
    RGM_CHILD      = 4,    // ��ͯ
    RGM_OTHER      = 5,    // �������Զ��壩
};

// �û�ģʽ
enum emUsersMode
{
	UM_EXPERT          = 0,    // �÷�����ʦ�û�
	UM_ADMIN           = 1,    // ����Ա�û�
	UM_NORMAL          = 2,    // ��ͨ�û�
};

// ����ģʽ
enum emTestFlag
{
	TF_NORMAL       = 0,    // ��������
	TF_REVIEW       = 1,    // ���ɲ���
	TF_ALERT        = 2,    // �쳣���ԣ�Ԥ�⣩
	TF_INVALID      = 3,    // ���ϲ���
};

#endif // _ALG_PLUS_DEFS_H_

