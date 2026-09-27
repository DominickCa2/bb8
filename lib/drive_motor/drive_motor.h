#define DRV_PIN1 18
#define DRV_PIN2 19
#define THR_LIMIT 200

class DriveMotor {
    public:
        void begin();
        void update();
};
