#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include "bgmplayer.h"

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(QWidget *parent = nullptr);
signals:
    void toLevelNum(int levelNum);
    void toStart();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    void loadBg(const QString &imgBg);
    void initWidget();
    void initSettingLabel();

    QIcon tintIcon(const QIcon& icon, const QColor& color = Qt::white, int size = 40); // 图标上色函数（将任意QIcon转为白色）


    //整体结构
    QVBoxLayout *m_mainLayout;

    QLabel *m_settingLabel; // 设置标签
    QHBoxLayout *m_difficultyControlLayout; // 难度切换布局
    QHBoxLayout *m_volumeControlLayout; // 全局音量布局
    QHBoxLayout *m_soundControlLayout; // 音效音量布局
    QHBoxLayout *m_bgmControlLayout; // BGM切换布局

    // 难度切换相关控件
    QLabel *m_difficultyControlTextLabel;
    QPushButton *m_prevDifficultyBtn; // 上一首按钮
    QPushButton *m_nextDifficultyBtn; // 下一首按钮
    QLabel *m_currentDifficultyLabel; // 曲目显示标签
    QVector<QString> m_difficultyList; // BGM 曲目列表

    // BGM切换相关函数声明
    void initDifficultyControlWidgets(); // 初始化难度控制控件
    void switchToPrevDifficulty(); // 切换上一首
    void switchToNextDifficulty(); // 切换下一首
    void updateDifficultyDisplay(); // 更新曲目显示

    // bgm音量控制相关控件
    QLabel *m_volumeTextLabel;
    QPushButton *m_muteBtn; // 静音按钮
    QSlider *m_volumeSlider; // 音量滑块
    QLabel *m_volumeLabel; // 音量显示标签

    void initVolumeControlWidgets(); // 初始化音量控制控件
    void onVolumeSliderValueChanged(int value);// 响应滑块值变化函数
    void syncVolumeSlider(int value);// 响应全局音量变化，同步滑块位置函数
    void initMuteButton(); // 初始化静音按钮
    void initVolumeSlider(); // 初始化音量滑块
    void updateMuteButtonIcon(bool isMuted); // 更新静音按钮图标函数声明

    // 音效音量控制相关控件
    QLabel *m_soundTextLabel;          // 音效标题
    QPushButton *m_soundMuteBtn;       // 音效静音按钮
    QSlider *m_soundSlider;            // 音效滑块
    QLabel *m_soundLabel;              // 音效音量显示

    void initSoundControlWidgets();
    void onSoundSliderValueChanged(int value);
    void syncSoundSlider(int value);
    void initSoundMuteButton();
    void initSoundSlider();
    void updateSoundMuteButtonIcon(bool isMuted);

    // BGM切换相关控件
    QLabel *m_bgmControlTextLabel;
    QPushButton *m_prevBgmBtn; // 上一首按钮
    QPushButton *m_nextBgmBtn; // 下一首按钮
    QLabel *m_bgmTitleLabel; // 曲目显示标签
    BgmPlayer *m_bgmPlayer; // BGM 播放器实例
    QVector<QString> m_bgmList; // BGM 曲目列表
    int m_currentBgmIndex = 0; // 当前曲目索引

    // BGM切换相关函数声明
    void initBgmControlWidgets(); // 初始化 BGM 控制控件
    void switchToPrevBgm(); // 切换上一首
    void switchToNextBgm(); // 切换下一首
    void updateBgmDisplay(); // 更新曲目显示

};

#endif // SETTINGWIDGET_H
