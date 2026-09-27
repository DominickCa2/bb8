
#define ADDR 0x68

class MPU {
public: 
    float acc_x, acc_y, acc_z;
    float rot_x, rot_y, rot_z;

    void begin();
    void update();
};
