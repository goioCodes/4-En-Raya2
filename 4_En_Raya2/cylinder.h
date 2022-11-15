#ifndef __CYLINDER_H_
#define __CYLINDER_H_

typedef struct cylinder {
    float radius;
    float height;
    int sectors;
    int stacks;
    float* verticesBase;
    float* verticesTop;
    float* verticesZigZag;
};

#endif // !__CYLINDER_H_
