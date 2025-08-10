/*
想到一个有趣的问题：

写一个STM32按键输入检测程序，
假设：有两个按键KEY1和KEY2连接至GPIOA PIN0和GPIOC PIN13引脚，按键按下时，GPIO端口高电平，按键松开，GPIO端口低电平；
按键KEY1和按键KEY2支持多种模式，
模式1：按键KEY1按下，之后松开，执行相应操作；
模式2：按键KEY2按下，立即执行操作，
模式3：按键KEY1或者按键KEY2长按，执行相应操作；
模式4：同时按下两个按键KEY1和KEY2，立即执行相应操作（不需要等松开）；

注意：软件需要考虑到按键的消抖问题
*/

/* 
思考过程
好的，用户需要写一个STM32的按键输入检测程序，涉及四个模式，同时要考虑消抖。让我仔细看一下具体要求。

首先，按键的连接情况是KEY1在PA0，KEY2在PC13。按下时高电平，松开低电平。所以需要配置这两个引脚为输入模式，可能有上拉或下拉电阻？根据描述，按键按下时端口为高，所以可能电路是按下时接VCC，松开时通过下拉电阻接地。因此，GPIO应该配置为下拉输入，或者在初始化时启用内部下拉电阻，这样默认是低电平，按下时变高。但具体要看硬件设计，这里用户已经说明，按下时高电平，所以配置为浮空输入，然后软件处理。或者可能需要内部下拉，以确保松开时稳定低电平。但根据问题描述，可能不需要，因为用户已经说明松开时低电平，所以可能外部已经有下拉电阻。因此GPIO模式可以设置为输入模式，无上拉下拉，或者根据实际硬件调整。

接下来是四个模式的处理：

1. 模式1：KEY1按下后松开才执行操作，即检测上升沿和下降沿？或者按下后松开触发。这类似于单击操作，需要在按键按下并释放后触发。

2. 模式2：KEY2按下立即执行，不需要等松开，所以检测到按下（上升沿）就触发。

3. 模式3：长按，需要检测按键持续按下的时间超过某个阈值，比如1秒。

4. 模式4：同时按下两个按键，立即执行，不管是否松开。

同时要考虑消抖。软件消抖通常通过延时检测，或者在检测到按键状态变化后等待一段时间再次确认状态。

接下来需要设计一个状态机来跟踪每个按键的状态，记录时间，处理不同模式。

可能的步骤：

- 初始化GPIO：KEY1和KEY2配置为输入，可能需要内部下拉（如果硬件没有外部下拉的话）。

- 定时器中断或系统定时器来定期扫描按键状态，比如每10ms检查一次，这样可以实现消抖。

- 对于每个按键，维护一个状态变量，记录当前状态（比如未按下、按下中、已触发等）以及按下的时间。

- 处理模式1：当检测到KEY1从按下到松开时，触发动作。需要检测按下和释放的过程，且在释放后才执行。

- 模式2：KEY2一旦按下，立即触发，但需要消抖，确保不是抖动。

- 模式3：长按需要检测按键按下的持续时间，超过设定的阈值（比如1000ms）则触发，并且在释放后可能需要处理，或者在达到阈值时立即处理一次。

- 模式4：同时检测两个按键是否都被按下，此时立即触发，不需要等待释放，同样需要消抖。

可能的实现方式：

使用定时器中断，每隔10ms执行一次按键扫描函数。每个按键有消抖计数器，当状态变化时，计数器开始计数，直到稳定后确认状态。例如，当检测到按键按下时，等待几个周期（比如5次，即50ms）确认状态是否仍然按下，避免抖动。

对于每个按键，需要记录当前状态（如释放、按下中、已确认按下等），以及按下的持续时间。例如，KEY1的长按处理需要累计按下的时间，超过阈值则触发模式3。

同时处理模式4，需要同时检测两个按键的状态，当两者都处于按下确认状态时，触发模式4。

需要注意优先级，比如模式4（同时按下）可能需要优先判断，避免与其他模式冲突。

代码结构可能包括：

- 按键结构体，包含引脚、端口、状态、计数器、按下时间等。

- 按键扫描函数，在定时器中断中调用，更新每个按键的状态。

- 在主循环中检查各个模式的条件是否满足，并执行相应操作。

消抖处理：当检测到按键状态变化时，启动计数器，只有连续多次检测到同一状态才认为有效。例如，当检测到按键从释放变为按下，开始计数，如果连续3次检测都是按下，则确认为有效按下，否则视为抖动。

对于模式1，需要在按键按下并释放后触发，所以要检测释放事件，并且在释放时确认之前是按下的状态，同时经过消抖处理。

对于长按，每次扫描时检查按下的持续时间是否超过阈值，如果是，则触发一次，可能需要设置标志位避免重复触发。

同时，处理同时按下需要考虑两个按键的按下状态是否都已经被确认，且在同时按下的情况下立即触发，不需要等待。

可能的问题点：

1. 消抖处理是否正确，避免误触发。

2. 各个模式之间的优先级和处理顺序是否正确，比如同时按下是否优先于其他模式。

3. 长按的判断如何与单击区分，避免在长按时触发单击事件。

4. 如何高效地管理多个按键的状态，避免代码冗余。

可能的解决方案：

- 使用面向对象的思想，为每个按键定义一个结构体，包含其相关参数。

- 使用状态机来处理每个按键的不同状态（如释放、按下等待确认、按下确认、释放等待确认等）。

- 使用时间戳或计数器来跟踪按键按下的持续时间，以判断长按。

- 在扫描函数中处理消抖和状态转换，主循环中根据状态执行相应的操作。

现在，根据这些思考，可以开始编写代码结构，比如定义按键的结构体，初始化GPIO，设置定时器中断，编写扫描函数，处理各个模式的条件判断。
*/

/* Deepseek */
#include "stm32fxxx.h"

// 按键定义
#define KEY1_PIN          GPIO_PIN_0
#define KEY1_PORT         GPIOA
#define KEY2_PIN          GPIO_PIN_13
#define KEY2_PORT         GPIOC

// 按键状态枚举
typedef enum {
    KEY_STATE_RELEASED,     // 按键释放
    KEY_STATE_PRESS_DB,     // 按下消抖中
    KEY_STATE_PRESSED,      // 按下确认
    KEY_STATE_RELEASE_DB    // 释放消抖中
} KeyState;

// 按键数据结构
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    KeyState state;
    uint32_t pressTime;
    uint8_t isPressed;
} Key_TypeDef;

// 全局按键实例
Key_TypeDef key1, key2;

// 消抖时间（30ms）
#define DEBOUNCE_TIME     30
// 长按时间阈值（1000ms）
#define LONG_PRESS_TIME   1000

void Key_Init(void);
void Key_Scan(Key_TypeDef* key);
void Key_Process(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    Key_Init();

    while (1) {
        Key_Scan(&key1);
        Key_Scan(&key2);
        Key_Process();
        HAL_Delay(10);  // 主循环延时10ms
    }
}

// 按键初始化
void Key_Init(void) {
    // KEY1 GPIO初始化
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = KEY1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;  // 假设外部无下拉电阻
    HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);

    // KEY2 GPIO初始化
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = KEY2_PIN;
    HAL_GPIO_Init(KEY2_PORT, &GPIO_InitStruct);

    // 初始化按键结构体
    key1.port = KEY1_PORT;
    key1.pin = KEY1_PIN;
    key1.state = KEY_STATE_RELEASED;
    key2.port = KEY2_PORT;
    key2.pin = KEY2_PIN;
    key2.state = KEY_STATE_RELEASED;
}

// 按键扫描函数（10ms周期调用）
void Key_Scan(Key_TypeDef* key) {
    uint8_t currentState = HAL_GPIO_ReadPin(key->port, key->pin);

    switch (key->state) {
    case KEY_STATE_RELEASED:
        if (currentState == GPIO_PIN_SET) {
            key->state = KEY_STATE_PRESS_DB;
            key->pressTime = HAL_GetTick();
        }
        break;

    case KEY_STATE_PRESS_DB:
        if ((HAL_GetTick() - key->pressTime) > DEBOUNCE_TIME) {
            if (currentState == GPIO_PIN_SET) {
                key->state = KEY_STATE_PRESSED;
                key->isPressed = 1;
                // 模式2：立即执行操作
                if (key == &key2) {
                    // 执行KEY2按下操作
                }
                // 检测模式4
                if (key1.isPressed && key2.isPressed) {
                    // 执行双键同时按下操作
                }
            } else {
                key->state = KEY_STATE_RELEASED;
            }
        }
        break;

    case KEY_STATE_PRESSED:
        if (currentState == GPIO_PIN_RESET) {
            key->state = KEY_STATE_RELEASE_DB;
            key->pressTime = HAL_GetTick();
        }
        // 检测长按
        else if ((HAL_GetTick() - key->pressTime) > LONG_PRESS_TIME) {
            // 模式3：长按操作（只触发一次）
            // 执行长按操作
            key->pressTime = HAL_GetTick(); // 重置时间防止重复触发
        }
        break;

    case KEY_STATE_RELEASE_DB:
        if ((HAL_GetTick() - key->pressTime) > DEBOUNCE_TIME) {
            if (currentState == GPIO_PIN_RESET) {
                key->state = KEY_STATE_RELEASED;
                key->isPressed = 0;
                // 模式1：释放后触发
                if (key == &key1) {
                    // 执行KEY1释放操作
                }
            } else {
                key->state = KEY_STATE_PRESSED;
            }
        }
        break;
    }
}

// 按键处理函数
void Key_Process(void) {
    // 模式3长按检测
    if (key1.isPressed && 
       (HAL_GetTick() - key1.pressTime) > LONG_PRESS_TIME) {
        // 执行KEY1长按操作
    }
    
    if (key2.isPressed && 
       (HAL_GetTick() - key2.pressTime) > LONG_PRESS_TIME) {
        // 执行KEY2长按操作
    }
}