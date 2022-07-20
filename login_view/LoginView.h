#ifndef LOGINVIEW_H
#define LOGINVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

class LoginCard;
class LoginOverlay;
class SignInView;
class SignUpView;

enum class LoginStatus
{
    SignIn = 0x20, // 登录
    SignUp // 注册
};

// 装载LoginCard
class LoginView : public QWidget
{
    Q_OBJECT
public:
    explicit LoginView(QWidget *parent = nullptr);
    ~LoginView();
    const SignInView* GetSignInView() const;
    const SignUpView* GetSignUpView() const;
protected:
    void Init();
    void paintEvent(QPaintEvent* event) override;
    void SignIn(const QString user, const QString pwd);
    void SignUp(const QString nickName, const QString user, const QString pwd);
private:
    LoginCard* m_pLoginCard;
    QPixmap m_backgroundPixmap;
};

// 装载LoginOverlay + SignInView + SignUpView
class LoginCard : public QWidget
{
    Q_OBJECT
public:
    explicit LoginCard(QWidget* parent = nullptr);
    ~LoginCard();
    const SignInView* GetSignInView() const;
    const SignUpView* GetSignUpView() const;
    LoginOverlay* GetOverlay() const;
protected:
    void Init();
    void paintEvent(QPaintEvent* event) override;
private:
    SignInView* m_pSignInView;
    SignUpView* m_pSignUpView;
    LoginOverlay* m_pOverlay;
};

// 图层
class LoginOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit LoginOverlay(QWidget* parent = nullptr);
    ~LoginOverlay();
    void SetPixmap(const QPixmap& pixmap);
protected:
    void Init();
    void paintEvent(QPaintEvent* event) override;
    void ChangeStatus();
private:
    const int m_nRadius = 8;
    bool m_bAni; // 是否正处于动画状态
    QPushButton* m_pButton;
    QPixmap m_backgroundPixmap;
    LoginStatus m_enStatus = LoginStatus::SignIn;
signals:
    /**
     * @brief StatusChanged 状态改变
     * @param status 当前最新的状态
     */
    void StatusChanged(LoginStatus status);
};

// 登录
class SignInView : public QWidget
{
    Q_OBJECT
public:
    explicit SignInView(QWidget* parent = nullptr);
    ~SignInView();

    /**
     * @brief Clear 清空界面
     */
    void Clear();
protected:
    void Init();
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief ButtonSignInClicked 登录按钮被按下
     */
    void ButtonSignInClicked();
private:
    QVBoxLayout* m_pVMainLayout;
    QLabel* m_pLabelTitle;
    QLineEdit* m_pEditUser;
    QLineEdit* m_pEditPwd;
    QPushButton* m_pBtnSignIn;
signals:
    /**
     * @brief Submitted 登录信息提交
     * @param user 用户名
     * @param pwd 密码
     */
    void Submitted(const QString user, const QString pwd);
};

// 注册
class SignUpView : public QWidget
{
    Q_OBJECT
public:
    explicit SignUpView(QWidget* parent = nullptr);
    ~SignUpView();

    /**
     * @brief Clear 清空界面
     */
    void Clear();
protected:
    void Init();
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief ButtonSignUpClicked 注册按钮被按下
     */
    void ButtonSignUpClicked();
private:
    QVBoxLayout* m_pVMainLayout;
    QLabel* m_pLabelTitle;
    QLineEdit* m_pEditNickName;
    QLineEdit* m_pEditUser;
    QLineEdit* m_pEditPwd;
    QPushButton* m_pBtnSignUp;
signals:
    /**
     * @brief Submitted 注册信息提交
     * @param nickName 昵称
     * @param user 用户名
     * @param pwd 密码
     */
    void Submitted(const QString nickName, const QString user, const QString pwd);
};

#endif // LOGINVIEW_H
