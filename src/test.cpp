//#include <GL/glaux.h>
#include <GL/glut.h>

float yRot = 0;
unsigned int nPre = 0;
GLfloat light_position[4] = {0,0,0,1};        //光位置
GLfloat light_diffuse[4] = {1,0,1,1};        //光的漫反色
GLfloat light_direction[4] = {-1, -2, -1, 1};    //光源聚光灯方向
GLfloat light_change[3] = {3,3,-1};            //光源移动位置。

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // 清楚颜色数据和深度数据（清屏）
    glColor3f(1.0,1.0,1.0);    
    glLoadIdentity();        //初始变换矩阵为单位矩阵

    glTranslated(0,-1,-5);    //平移0,0,-5向Z负方向平移5个单位

    glPushMatrix();        //压缩矩阵，设定光源的位置
    glDisable(GL_LIGHTING);    
    glRotatef(yRot, 0, 1, 0);    //光源的旋转
    glTranslatef(light_change[0],light_change[1],light_change[2]);    //光源的位置
    glTranslatef(light_position[0], light_position[1],light_position[2]);    //光源位置
    
    glutSolidSphere(0.1, 4,4);//利用库函数绘制一个半径为1的球体。表示光源在这里
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glPopMatrix();
    light_direction[0] = -light_change[0];
    light_direction[1] = -light_change[1];
    light_direction[2] = -light_change[2];
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glPopMatrix();        //光源设置完毕

    glPushMatrix();            //要入矩阵，绘制地面
    glBegin(GL_QUADS);
    glVertex3f(-3,0,3);
    glVertex3f(3,0,3);
    glVertex3f(3,0,-3);
    glVertex3f(-3,0,-3);
    glEnd();
    glPopMatrix();            //地面绘制完毕

    glPushMatrix();            //压入变换矩阵那
    glutSolidSphere(1, 30,30);//利用库函数绘制一个半径为1的球体。
    glPopMatrix();            //弹出矩阵。

    glutSwapBuffers();            //交换缓冲区。显示图形
}

//初始化
void init (void) 
{
    //srand(0);    随机种子
    glClearColor (0.5, 0.5, 0.5, 0.0);    //清理颜色，为黑色，（也可认为是背景颜色）
    
    GLfloat light_param[] = { 1, 1, 1, 1.0 };    //初始化光参数。
    //glLightfv(GL_LIGHT0, GL_AMBIENT, light_param);    //设置光为环境光。
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0); //设置聚光的范围为45度。
    glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,2.0);    //设置聚光灯的聚光强度。
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, light_param);    //设置材质的漫反射属性
    //glMaterialfv(GL_FRONT, GL_AMBIENT, light_param);    //设置材质的环境属性
    glMaterialf(GL_FRONT, GL_SHININESS, 64);    //设置材质的高光属性
    

    glEnable(GL_LIGHTING);        //启动光照
    glEnable(GL_LIGHT0);        //启动GL_LIGHTO光
    glCullFace(GL_BACK);        //剔除背面
    glEnable(GL_CULL_FACE);        //启动剔除
    glDepthFunc(GL_LEQUAL);        //深度检测为小于等于
    glEnable(GL_DEPTH_TEST);    //启动深度检测
    glFrontFace(GL_CCW);        //定义逆时针为正面

}

//当窗口大小改变时，会调用这个函数
void reshape(GLsizei w,GLsizei h)
{
    //这里小说明一下：矩阵模式是不同的，他们各自有一个矩阵。投影相关
    //只能用投影矩阵。(只是目前情况下哦，等我学多了可能就知道为什么了。)

    glViewport(0,0,w,h);        //设置视口
    glMatrixMode(GL_PROJECTION);    //设置矩阵模式为投影变换矩阵，
    glLoadIdentity();                //变为单位矩阵
    gluPerspective(60, (GLfloat)w / h, 0, 1000);    //设置投影矩阵
    //glOrtho(-6.0, 6.0, -6.0 * h / w, 6.0* h / w, -10, 10);    //为了不变形，则要长和宽成比例
    glMatrixMode(GL_MODELVIEW);        //设置矩阵模式为视图矩阵(模型)
    glLoadIdentity();                //变为单位矩阵
}

void Keyboard(unsigned char key, int x, int y)
{
}

//闲置函数，当主循环空闲时就会调用这个函数
void MyIdle(void)
{
//    unsigned int nNow = ::GetTickCount();
	unsigned int nNow = 100;
	float delt = 100;//nNow - nPre;
    if (delt >  100)
    {
        nPre = nNow;
        yRot += 0.1 * delt;    //旋转角度
        /*light_diffuse[0] = (rand() % 255) / 255.0;//太花不用了。
        light_diffuse[1] = (rand() % 255) / 255.0;
        light_diffuse[2] = (rand() % 255) / 255.0;*/
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);        //Opnegl初始化
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA); //设置显示模式为双缓冲，RGEBA

    glutInitWindowSize (800, 600);            //窗口大小
    glutInitWindowPosition (100, 100);        //窗口位置
    glutCreateWindow ("hello");                //创建一个标题为hello的窗口
    init ();                                //初始化资源,这里一定要在创建窗口以后，不然会无效。
    glutDisplayFunc(display);                //窗口大小改变时的回调
    glutReshapeFunc(reshape);                //绘制图形时的回调
    glutKeyboardFunc(Keyboard);
    glutIdleFunc(MyIdle);
    glutMainLoop();                            //主循环。
    return 0;  
}
