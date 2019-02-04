#ifndef __LIST_H__
#define __LIST_H__

#define LIST_EMPTY (1)
#define	list_head(h) list_t h = { &h, &h }
#define list_t struct struct_list

list_t {
	list_t	*n;
	list_t	*p;
};

static inline void list_init (list_t *l)
{
	l->n = l;
	l->p = l;
}

static inline void list_ins (list_t *f, list_t *l, list_t *p, list_t *n)
{
	l->n = n;
	f->p = p;
	p->n = f;
	n->p = l;
}

static inline void list_del(list_t *p, list_t *n)
{
	n->p = p;
	p->n = n;
}

static inline void list_prepend (list_t *e, list_t *l)
{
	list_ins(e, e, l, l->n);
}

static inline void list_append (list_t *e, list_t *l)
{
	list_ins(e, e, l->p, l);
}

static inline void list_remove (list_t *e)
{
	list_del(e->p, e->n);
}

static inline int list_empty (list_t *l)
{
	return l->n == l;
}

#define list_add(e,l) list_append(e,l)
#define	list_for(_p,_l)	for (_p = (_l)->n; _p != (_l); _p = _p->n)
#define	list_entry(_p,_t,_m) ((_t *)((char *)(&_p->n)-(unsigned long)(&((_t *)0)->_m)))

#endif // __LIST_H__

