void lineFollow(char inst) {
    do {
        Status cur_status = de(inst);
        PID(cur_status);
        go();
    } while (!position.update_pos())
}


Status de(char inst) {
    Status cur_status;
    cur_status.forward = cur_status.right = cur_status.left = false
    switch (inst) {
        case RIGHT:
            cur_status.right = true;
            return cur_status
        case LEFT:
            cur_status.left = true;
            return cur_status
        case FORWARD:
            cur_status.forward = true;
            return cur_status
        case POS:
            Serial.println(position.getCurPos());
            break;
        case HALT:
        default:
            Serial.println("HALT");
            break;
    }
}

static inline void echo(String inst) {
    Serial.println(inst);
}

void PID_program()
{
  int MAX_SPEED = 18;

  read_sensor();
  previousError = error; // save previous error for differential
  error = avgSensor; // Count how much robot deviate from center
  totalError += error;

  power = (Kp * error) + (Kd * (error - previousError)); // pid hesaplamasi

  if ( power > MAX_SPEED ) {
    power = MAX_SPEED;
  }
  if ( power < -MAX_SPEED ) {
    power = -MAX_SPEED;
  }

  if (power > 0) // Turn left
  {
    PWM_Right = MAX_SPEED + (5 * abs(int(power))) / 10;
    PWM_Left = MAX_SPEED -  (6 * abs(int(power))) / 10;
  }
  else // Turn right
  {
    PWM_Right = MAX_SPEED - (6 * abs(int(power))) / 10;
    PWM_Left = MAX_SPEED +  (5 * abs(int(power))) / 10;
  }

}