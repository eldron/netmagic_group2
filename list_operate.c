/*..........������Ĳ���..........*/
#include"nmac.h"

Link Creat_List() //�����յ�����
{
	Link H;
	H = malloc(sizeof(Node));
	if (H)
		H->next = NULL;
	return H;
}

void Destroy_List(Link H) //���ٵ�����
{
	Link p, q;
	p = H;
	while (p) {
		q = p;
		p = p->next;
		free(q);
	}
	H = NULL;
}

void Clear_List(Link H) //��յ�����, ��Ϊ�ձ�
{
	Link p, q;
	p = H;
	while (p) {
		q = p;
		p = p->next;
		free(q);
	}
	H->next = NULL;
}

int List_Empty(Link H) //�ж������Ƿ�Ϊ�ձ�
{
	if (H->next) {
		return 0;
	} else {
		return 1;
	}
}

int Length_List(Link H) //��������
{
	Link p = H;
	int count = -1; //��ͷ������Դ�-1��ʼ
	while (p) {
		p = p->next;
		count++;
	}
	return (count);
}

int Locate_list_Pos(Link H, int i) //��������Ų���Ԫ�أ������ر���id
{
	Link p;
	int j = 0;
	p = H;
	while (p && j < i) {
		p = p->next;
		j++;
	}
	if (j != i || !p) {
		printf("Error i!\n");
		return 0;
	}
	return p->entry.id;
}

Link Locate_List_Value(Link H, u_int8_t id) //��ֵ����
{ //������id���ұ���
	Link p = H->next;
	printf("ff\n");
	while (p && p->entry.id != id)
		p = p->next;
	return p;
}

void Delete_Elem(Link H, u_int8_t id) //��idɾ������
{
	Link q;
	Link p = H;
	while (p->next) {
		q = p->next; //qָ��p�ĺ��

		if (q->entry.id == id) {
			break;
		}
		p = q;
	}
	p->next = q->next;
	free(q);
	printf("Delete a entry node success!\n");
}

void Add_Elem(Link H, //�����½ڵ㣬ͷ�巨
		u_int8_t id, u_int32_t addr, //������ʼ��ַ
		u_int32_t dst_ip, u_int16_t trans_port, u_int8_t forward_port) {
	Link s;
	s = malloc(sizeof(Node));
	s->entry.id = id;
	s->entry.addr = addr;
	s->entry.dst_ip = dst_ip;
	s->entry.trans_port = trans_port;
	s->entry.forward_port = forward_port;

	s->next = H->next;
	H->next = s;
	printf("Add a entry node success!\n");
}
