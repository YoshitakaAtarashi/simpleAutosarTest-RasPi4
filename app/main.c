/**
 * Trampoline AUTOSAR OS Application
 * Raspberry Pi HDMI Display Test
 */

#include "tpl_os.h"
#include "framebuffer.h"
#include "uart_comm.h"
#include <stdint.h>

/* Output mode selection */
#define USE_HDMI  1  /* Set to 1 for HDMI, 0 for UART */

#if USE_HDMI
    #define output_init()       fb_init()
    #define output_puts(s)      fb_puts(s)
    #define output_putc(c)      fb_putc(c)
    #define output_put_hex(v)   fb_put_hex(v)
    #define output_put_dec(v)   fb_put_dec(v)
#else
    #define output_init()       uart_init(115200)
    #define output_puts(s)      uart_puts(s)
    #define output_putc(c)      uart_putc(c)
    #define output_put_hex(v)   uart_put_hex(v)
    #define output_put_dec(v)   uart_put_dec(v)
#endif

/* グローバル変数 */
static uint32_t task_counter = 0;
static uint32_t counter_100ms = 0;
static uint32_t counter_1000ms = 0;
static uint8_t led_state = 0;
static char rx_buffer[128];
static volatile uint8_t rx_ready = 0;

/**
 * StartupHook
 * OS起動時に1度だけ呼ばれる
 */
void StartupHook(void)
{
    /* 出力デバイス初期化 (HDMI or UART) */
    output_init();
    
    /* GPIO初期化（LED用） */
    gpio_init();
    
    /* 起動メッセージ送信 */
    output_puts("\n");
    output_puts("================================================\n");
    output_puts(" Trampoline AUTOSAR OS on Raspberry Pi 4\n");
#if USE_HDMI
    output_puts(" HDMI Display Test\n");
#else
    output_puts(" Serial Communication Test\n");
#endif
    output_puts("================================================\n");
    output_puts(" Build: ");
    output_puts(__DATE__);
    output_puts(" ");
    output_puts(__TIME__);
    output_puts("\n");
    output_puts("================================================\n");
    output_puts("\n");
}

/**
 * ErrorHook
 * エラー発生時に呼ばれる
 */
void ErrorHook(StatusType error)
{
    output_puts("ERROR: ");
    output_put_hex(error);
    output_puts("\n");
    
    /* エラー時はLEDを高速点滅 */
    while(1) {
        gpio_led_toggle();
        delay_ms(100);
    }
}

/**
 * ShutdownHook
 * OS終了時に呼ばれる
 */
void ShutdownHook(StatusType error)
{
    output_puts("\nSystem Shutdown. Error code: ");
    output_put_hex(error);
    output_puts("\n");
}

/**
 * TASK: TaskSerial
 * 1秒周期でシリアルメッセージを送信
 */
TASK(TaskSerial)
{
    StatusType status;
    char msg[64];
    
    /* UART排他制御 */
    status = GetResource(ResUart);
    
    if (status == E_OK) {
        /* カウンタをインクリメント */
        task_counter++;
        
        /* メッセージ作成 */
        output_puts("[TaskSerial] Count: ");
        output_put_dec(task_counter);
        output_puts(" | Uptime: ");
        output_put_dec(task_counter);
        output_puts(" sec\n");
        
        ReleaseResource(ResUart);
    }
    
    TerminateTask();
}

/**
 * TASK: TaskBlink
 * 500ms周期でLEDを点滅
 */
TASK(TaskBlink)
{
    /* LED状態を反転 */
    gpio_led_toggle();
    led_state = !led_state;
    
    TerminateTask();
}

/**
 * TASK: TaskProcess
 * データ処理タスク（イベント駆動）
 */
TASK(TaskProcess)
{
    EventMaskType event;
    StatusType status;
    
    /* イベント待機 */
    status = WaitEvent(EvtDataReady);
    
    if (status == E_OK) {
        /* イベント取得 */
        GetEvent(TaskProcess, &event);
        
        if (event & EvtDataReady) {
            /* データ処理 */
            status = GetResource(ResUart);
            
            if (status == E_OK) {
                output_puts("[TaskProcess] Received: ");
                output_puts(rx_buffer);
                output_puts("\n");
                
                ReleaseResource(ResUart);
            }
            
            /* イベントクリア */
            ClearEvent(EvtDataReady);
            
            /* 受信バッファクリア */
            rx_ready = 0;
        }
    }
    
    TerminateTask();
}

/**
 * TASK: Task100ms
 * 100ms周期でカウンターをインクリメントして表示
 */
TASK(Task100ms)
{
    StatusType status;
    
    /* 排他制御 */
    status = GetResource(ResUart);
    
    if (status == E_OK) {
        /* カウンタをインクリメント */
        counter_100ms++;
        
        /* カウンター表示 */
        output_puts("[100ms Task] Counter: ");
        output_put_dec(counter_100ms);
        output_puts("\n");
        
        ReleaseResource(ResUart);
    }
    
    TerminateTask();
}

/**
 * TASK: Task1000ms
 * 1000ms周期でカウンターをインクリメントして表示
 */
TASK(Task1000ms)
{
    StatusType status;
    
    /* 排他制御 */
    status = GetResource(ResUart);
    
    if (status == E_OK) {
        /* カウンタをインクリメント */
        counter_1000ms++;
        
        /* カウンター表示 */
        output_puts("[1000ms Task] Counter: ");
        output_put_dec(counter_1000ms);
        output_puts(" | 100ms Counter: ");
        output_put_dec(counter_100ms);
        output_puts("\n");
        
        ReleaseResource(ResUart);
    }
    
    TerminateTask();
}

/**
 * ISR: IsrUartRx
 * UART受信割り込みハンドラ
 */
ISR(IsrUartRx)
{
    static uint8_t rx_idx = 0;
    char ch;
    
    /* 受信データ読み取り */
    if (uart_available()) {
        ch = uart_getc();
        
        /* エコーバック */
        uart_putc(ch);
        
        /* バッファに格納 */
        if (ch == '\r' || ch == '\n') {
            /* 改行で終端 */
            rx_buffer[rx_idx] = '\0';
            rx_idx = 0;
            rx_ready = 1;
            
            uart_puts("\r\n");
            
            /* データ処理タスクを起動 */
            ActivateTask(TaskProcess);
            SetEvent(TaskProcess, EvtDataReady);
        } else if (rx_idx < sizeof(rx_buffer) - 1) {
            rx_buffer[rx_idx++] = ch;
        }
    }
}

/**
 * main関数
 * Trampolineではここからスタートし、StartOS()を呼び出す
 */
int main(void)
{
    uint32_t loop_counter = 0;
    uint32_t last_second = 0;
    
    /* AUTOSAR OS起動 */
    StartOS(OSDEFAULTAPPMODE);
    
    /* 
     * StartOS()がスタブの場合、ここに到達する
     * シンプルなポーリングループでカウンター表示
     */
    
    while(1) {
        loop_counter++;
        
        /* 約1秒ごとに表示（簡易的な遅延） */
        if (loop_counter % 1000000 == 0) {
            last_second++;
            
            /* カウンター表示 */
            output_puts("[Loop] Second: ");
            output_put_dec(last_second);
            output_puts(" | Counter: ");
            output_put_dec(loop_counter / 1000000);
            output_puts("\n");
            
            /* LED点滅 */
            gpio_led_toggle();
            
            /* Task100msの代替デバッグ表示 */
            counter_100ms++;
            output_puts("  [100ms sim] Counter: ");
            output_put_dec(counter_100ms);
            output_puts("\n");
        }
        
        /* 10秒ごとに1000msカウンター表示 */
        if (loop_counter % 10000000 == 0) {
            counter_1000ms++;
            output_puts("  [1000ms sim] Counter: ");
            output_put_dec(counter_1000ms);
            output_puts(" | 100ms: ");
            output_put_dec(counter_100ms);
            output_puts("\n");
        }
    }
    
    return 0;
}
