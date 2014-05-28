/*..........单链表的操作..........*/
#include"nmac.h"

Link Creat_List() //创建空单链表
{
	Link H;
	H = malloc(sizeof(Node));
	if (H)
		H->next = NULL;
	return H;
}

void Destroy_List(Link H) //销毁单链表
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

void Clear_List(Link H) //清空单链表, 置为空表
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

int List_Empty(Link H) //判断链表是否为空表
{
	if (H->next) {
		return 0;
	} else {
		return 1;
	}
}

int Length_List(Link H) //求单链表长度
{
	Link p = H;
	int count = -1; //有头结点所以从-1开始
	while (p) {
		p = p->next;
		count++;
	}
	return (count);
}

int Locate_list_Pos(Link H, int i) //按链表序号查找元素，并返回表项id
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

Link Locate_List_Value(Link H, u_int8_t id) //按值查找
{ //按表项id查找表项
	Link p = H->next;
	printf("ff\n");
	while (p && p->entry.id != id)
		p = p->next;
	return p;
}

void Delete_Elem(Link H, u_int8_t id) //按id删除表项
{
	Link q;
	Link p = H;
	while (p->next) {
		q = p->next; //q指向p的后继

		if (q->entry.id == id) {
			break;
		}
		p = q;
	}
	p->next = q->next;
	free(q);
	printf("Delete a entry node success!\n");
}

void Add_Elem(Link H, //插入新节点，头插法
		u_int8_t id, u_int32_t addr, //表项起始地址
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
