#include "LoginView.h"
#include <QPainter>
#include <QStyleOption>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QPainterPath>
#ifdef QT_DEBUG
#include <QDebug>
#endif

static int nScreenWidth = 0;
static int nScreenHeight = 0;
static int nDuration = 300; // 动画时间(单位ms)

LoginView::LoginView(QWidget *parent) : QWidget(parent)
{
    nScreenWidth = QApplication::primaryScreen()->geometry().width();
    nScreenHeight = QApplication::primaryScreen()->geometry().height();
    setFixedSize(nScreenWidth, nScreenHeight);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    Init();
}

LoginView::~LoginView()
{

}

const SignInView *LoginView::GetSignInView() const
{
    return m_pLoginCard->GetSignInView();
}

const SignUpView *LoginView::GetSignUpView() const
{
    return m_pLoginCard->GetSignUpView();
}

void LoginView::Init()
{
    setObjectName(QStringLiteral("login_view"));
    setStyleSheet(QStringLiteral("QWidget#login_view{border:none;}"));
    m_backgroundPixmap.load(":/res/background.png");
    m_backgroundPixmap = m_backgroundPixmap.scaled(nScreenWidth, nScreenHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_pLoginCard = new LoginCard(this);
    m_pLoginCard->GetOverlay()->SetPixmap(m_backgroundPixmap);
    m_pLoginCard->move( (width() - m_pLoginCard->width()) / 2,
                        (height() - m_pLoginCard->height()) / 2  );

    connect(GetSignInView(), &SignInView::Submitted, this, &LoginView::SignIn);
    connect(GetSignUpView(), &SignUpView::Submitted, this, &LoginView::SignUp);
    showFullScreen();
}

void LoginView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    p.drawPixmap(0, 0, m_backgroundPixmap);
    QWidget::paintEvent(event);
}

void LoginView::SignIn(const QString user, const QString pwd)
{
    // TODO: 执行登录的操作
}

void LoginView::SignUp(const QString nickName, const QString user, const QString pwd)
{
    // TODO: 执行注册的操作
}

////////////////////////////////////////////////////////////////////////////////
/// \brief LoginCard
//////////////////////////////////////////////////////////////////////////////////////

LoginCard::LoginCard(QWidget *parent) : QWidget(parent)
{
    Init();
}

LoginCard::~LoginCard()
{

}

const SignInView *LoginCard::GetSignInView() const
{
    return m_pSignInView;
}

const SignUpView *LoginCard::GetSignUpView() const
{
    return m_pSignUpView;
}

LoginOverlay *LoginCard::GetOverlay() const
{
    return m_pOverlay;
}

void LoginCard::Init()
{
    setObjectName(QStringLiteral("login_card"));
    setStyleSheet(QStringLiteral("QWidget#login_card{border-radius:8px;background-color:white;}"));
    setFixedSize(nScreenWidth * 0.8, nScreenHeight * 0.8);

    m_pSignInView= new SignInView(this);
    m_pSignInView->move(width() / 2, 0);

    m_pSignUpView = new SignUpView(this);
    m_pSignUpView->move(width() / 2, 0);
    m_pSignUpView->hide();

    m_pOverlay = new LoginOverlay(this);
    m_pOverlay->move(0, 0);

    connect(m_pOverlay, &LoginOverlay::StatusChanged, this, [&](LoginStatus status){
        QPropertyAnimation* an = nullptr;
        m_pSignInView->Clear();
        m_pSignUpView->Clear();
        if(status == LoginStatus::SignIn)
        {
            // 此时应该显示登录窗口，登录窗口在右边
            // 登录窗口从左往右移动
            m_pSignInView->move(width() / 2, 0);
            m_pSignUpView->move(0, 0);
            m_pSignInView->hide();
            an = new QPropertyAnimation(m_pSignUpView, "geometry");
            connect(an, &QPropertyAnimation::finished, this, [&, an]{
                delete an;
                m_pSignUpView->hide();
                m_pSignInView->show();
            });
            an->setDuration(nDuration);
            an->setStartValue(QRect(m_pSignUpView->pos().x(), m_pSignUpView->pos().y(), m_pSignUpView->width(), m_pSignUpView->height()));
            an->setEndValue(QRect(m_pSignUpView->width(), m_pSignUpView->pos().y(), m_pSignUpView->width(), m_pSignUpView->height()));
        }
        else
        {
            // 此时应该显示注册窗口，注册窗口在左边
            m_pSignInView->move(width() / 2, 0);
            m_pSignUpView->move(0, 0);
            m_pSignUpView->hide();
            an = new QPropertyAnimation(m_pSignInView, "geometry");
            connect(an, &QPropertyAnimation::finished, this, [&, an]{
                delete an;
                m_pSignInView->hide();
                m_pSignUpView->show();
            });
            an->setDuration(nDuration);
            an->setStartValue(QRect(m_pSignInView->pos().x(), m_pSignInView->pos().y(), m_pSignInView->width(), m_pSignInView->height()));
            an->setEndValue(QRect(0, m_pSignInView->pos().y(), m_pSignInView->width(), m_pSignInView->height()));
        }

        an->start();
    });

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(Qt::gray);
    shadow->setBlurRadius(30);
    setGraphicsEffect(shadow);
    setContentsMargins(1,1,1,1);
}

void LoginCard::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief LoginOverlay
///
LoginOverlay::LoginOverlay(QWidget *parent) : QWidget(parent), m_bAni(false)
{
    qRegisterMetaType<LoginStatus>("LoginStatus");
    setFixedSize(parentWidget()->width() / 2,
                 parentWidget()->height());
    Init();
}

LoginOverlay::~LoginOverlay()
{

}

void LoginOverlay::SetPixmap(const QPixmap &pixmap)
{
    m_backgroundPixmap = pixmap;
}

void LoginOverlay::Init()
{
    setObjectName(QStringLiteral("login_overlay"));
    setStyleSheet(QStringLiteral("QWidget#login_overlay{border-radius:8px;}"));
    m_pButton = new QPushButton(this);
    m_pButton->setCursor(Qt::PointingHandCursor);
    m_pButton->setStyleSheet(QStringLiteral("QPushButton{font-size:22px;font-family:Microsoft Yahei;color:white;border:none;border-radius:30px;background-color:#409EFF;}"
                                            "QPushButton:hover{background-color:rgb(102,177,255);}"
                                            "QPushButton:pressed{background-color:rgb(58,142,230)};"));
    m_pButton->setFixedSize(width() * 0.36, 60);
    if(m_enStatus == LoginStatus::SignIn)
    {
        m_pButton->setText(QStringLiteral("注册"));
    }
    else
    {
        m_pButton->setText(QStringLiteral("登录"));
    }
    m_pButton->move( (width() - m_pButton->width()) / 2,
                     (height() - m_pButton->height()) / 2  );
    connect(m_pButton, &QPushButton::clicked, this, &LoginOverlay::ChangeStatus);
    raise();
}

void LoginOverlay::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
#ifdef QT_DEBUG
    qDebug() << "current x is " << pos().x() << "  current y is " << pos().y();
    qDebug() << "mapToGlobal x is " << mapToGlobal(pos()).x() << "  mapToGlobal y is " << mapToGlobal(pos()).y();
#endif
    QPixmap pixmap = m_backgroundPixmap.copy(mapToGlobal(QPoint(0, 0)).x(),
                                             mapToGlobal(QPoint(0, 0)).y(),
                                             width(),
                                             height());
    QPainterPath path;
    QRect rect(0, 0, pixmap.width(), pixmap.height());
    path.addRoundedRect(rect, m_nRadius, m_nRadius);
    path.setFillRule(Qt::WindingFill);
    if(m_enStatus == LoginStatus::SignIn)
    {
        path.addRect(width() - m_nRadius, 0, m_nRadius, m_nRadius);
        path.addRect(width() - m_nRadius, height() - m_nRadius, m_nRadius, m_nRadius);
    }
    else
    {
        path.addRect(0, height() - m_nRadius, m_nRadius, m_nRadius);
        path.addRect(0, 0, m_nRadius, m_nRadius);
    }
    p.setClipPath(path);
    p.drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
    QWidget::paintEvent(event);
}

void LoginOverlay::ChangeStatus()
{
    if(m_bAni)
        return;
    m_bAni = true;
    QSequentialAnimationGroup *pOpacityGroup = new QSequentialAnimationGroup(this);
    QPropertyAnimation* an = new QPropertyAnimation(this, "geometry");
    QPropertyAnimation* an2 = new QPropertyAnimation(m_pButton, "geometry");
    connect(an, &QPropertyAnimation::finished, this, [&, an]()mutable{
        delete an;
        an = nullptr;
        m_bAni = false;
    });
    QPropertyAnimation* an3 = new QPropertyAnimation(m_pButton, "geometry");


    an->setDuration(nDuration);
    an->setStartValue(QRect(pos().x(), pos().y(), width(), height()));

    an2->setDuration(nDuration / 2);
    an2->setStartValue(QRect(m_pButton->pos().x(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));

    an3->setDuration(nDuration / 2);

    connect(an2, &QPropertyAnimation::finished, this, [&, an2]()mutable{
        delete an2;
        an2 = nullptr;
        if(m_enStatus == LoginStatus::SignIn)
        {
            m_pButton->move(-m_pButton->width(), m_pButton->y());
            m_pButton->setText(QStringLiteral("注册"));
        }
        else
        {
            m_pButton->move(width() + m_pButton->width(), m_pButton->y());
            m_pButton->setText(QStringLiteral("登录"));
        }
    });

    connect(an3, &QPropertyAnimation::finished, this, [&, an3]()mutable{
        delete an3;
        an3 = nullptr;
    });

    if(m_enStatus == LoginStatus::SignIn)
    {
        // 从左移动到右
        m_enStatus = LoginStatus::SignUp;
        an->setEndValue(QRect(width(), 0, width(), height()));
        an2->setEndValue(QRect(-m_pButton->width(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));
        an3->setStartValue(QRect(width() + m_pButton->width(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));
    }
    else
    {
        // 从右边移动到左边
        m_enStatus = LoginStatus::SignIn;
        an->setEndValue(QRect(0, 0, width(), height()));
        an2->setEndValue(QRect(width() + m_pButton->width(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));
        an3->setStartValue(QRect(-m_pButton->width(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));
    }
    an3->setEndValue(QRect(m_pButton->pos().x(), m_pButton->pos().y(), m_pButton->width(), m_pButton->height()));
    emit StatusChanged(m_enStatus);
    pOpacityGroup->addAnimation(an2);
    pOpacityGroup->addAnimation(an3);
    an->start();
    pOpacityGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

/////////////////////////////////////////////////////////////////
/// \brief SignInView
////
SignInView::SignInView(QWidget *parent) : QWidget(parent)
{
    Init();
}

SignInView::~SignInView()
{

}

void SignInView::Clear()
{
    m_pEditPwd->clear();
    m_pEditUser->clear();
}

void SignInView::Init()
{
    setFixedSize(parentWidget()->width() / 2,
                 parentWidget()->height());
    setObjectName(QStringLiteral("sign_in_view"));
    setStyleSheet(QStringLiteral("QWidget#sign_in_view{border:none;background-color:white;border-top-right-radius:8px;border-bottom-right-radius:8px;}"));
    m_pVMainLayout = new QVBoxLayout(this);
    m_pLabelTitle = new QLabel(QStringLiteral("登录"));
    m_pEditUser = new QLineEdit(this);
    m_pEditUser->setPlaceholderText(QStringLiteral("账号"));
    m_pEditUser->setFixedSize(width() * 0.6, 65);
    m_pEditPwd = new QLineEdit(this);
    m_pEditPwd->setEchoMode(QLineEdit::EchoMode::Password);
    m_pEditPwd->setPlaceholderText(QStringLiteral("密码"));
    m_pEditPwd->setFixedSize(width() * 0.6, 65);
    m_pBtnSignIn = new QPushButton(QStringLiteral("登录"), this);
    m_pBtnSignIn->setCursor(Qt::PointingHandCursor);
    m_pBtnSignIn->setFixedSize(m_pEditUser->width() * 0.6, 60);

    m_pLabelTitle->setStyleSheet(QStringLiteral("QLabel{font-size:40px;font-family:Microsoft Yahei;color:#606266;}"));
    m_pEditUser->setStyleSheet(QStringLiteral("QLineEdit{padding-left:25px;padding-right:25px;font-size:20px;font-family:Microsoft Yahei;border-radius:3px;border:1px solid #dcdfe6;color:#606266;}"
                                              "QLineEdit:hover{border:1px solid #909399;}"
                                              "QLineEdit:focus{border:1px solid #409EFF;}"));
    m_pEditPwd->setStyleSheet(QStringLiteral("QLineEdit{padding-left:25px;padding-right:25px;font-size:20px;font-family:Microsoft Yahei;border-radius:3px;border:1px solid #dcdfe6;color:#606266;}"
                                              "QLineEdit:hover{border:1px solid #909399;}"
                                              "QLineEdit:focus{border:1px solid #409EFF;}"));
    m_pBtnSignIn->setStyleSheet(QStringLiteral("QPushButton{font-size:22px;font-family:Microsoft Yahei;color:white;border:none;border-radius:30px;background-color:#409EFF;}"
                                            "QPushButton:hover{background-color:rgb(102,177,255);}"
                                            "QPushButton:pressed{background-color:rgb(58,142,230)};"));

    for(auto & edit : findChildren<QLineEdit*>())
        edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_pLabelTitle->adjustSize();
    m_pVMainLayout->addStretch();
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pLabelTitle, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pEditUser, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pEditPwd, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pBtnSignIn, 0, Qt::AlignCenter);
    m_pVMainLayout->addStretch();

    connect(m_pBtnSignIn, &QPushButton::clicked, this, &SignInView::ButtonSignInClicked);
}

void SignInView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void SignInView::ButtonSignInClicked()
{
    emit Submitted(m_pEditUser->text(), m_pEditPwd->text());
}

//////////////////////////////////////////////////////////////////////
/// \brief SignUpView
///
SignUpView::SignUpView(QWidget *parent) : QWidget(parent)
{
    Init();
}

SignUpView::~SignUpView()
{

}

void SignUpView::Clear()
{
    m_pEditNickName->clear();
    m_pEditPwd->clear();
    m_pEditUser->clear();
}

void SignUpView::Init()
{
    setFixedSize(parentWidget()->width() / 2,
                 parentWidget()->height());
    setObjectName(QStringLiteral("sign_up_view"));
    setStyleSheet(QStringLiteral("QWidget#sign_up_view{border:none;background-color:white;border-top-left-radius:8px;border-bottom-left-radius:8px;}"));
    m_pVMainLayout = new QVBoxLayout(this);
    m_pLabelTitle = new QLabel(QStringLiteral("注册"));
    m_pEditNickName = new QLineEdit(this);
    m_pEditNickName->setPlaceholderText(QStringLiteral("昵称"));
    m_pEditNickName->setFixedSize(width() * 0.6, 65);
    m_pEditUser = new QLineEdit(this);
    m_pEditUser->setPlaceholderText(QStringLiteral("账号"));
    m_pEditUser->setFixedSize(width() * 0.6, 65);
    m_pEditPwd = new QLineEdit(this);
    m_pEditPwd->setEchoMode(QLineEdit::EchoMode::Password);
    m_pEditPwd->setPlaceholderText(QStringLiteral("密码"));
    m_pEditPwd->setFixedSize(width() * 0.6, 65);
    m_pBtnSignUp = new QPushButton(QStringLiteral("注册"), this);
    m_pBtnSignUp->setCursor(Qt::PointingHandCursor);
    m_pBtnSignUp->setFixedSize(m_pEditUser->width() * 0.6, 60);

    m_pLabelTitle->setStyleSheet(QStringLiteral("QLabel{font-size:40px;font-family:Microsoft Yahei;color:#606266;}"));
    m_pEditNickName->setStyleSheet(QStringLiteral("QLineEdit{padding-left:25px;padding-right:25px;font-size:20px;font-family:Microsoft Yahei;border-radius:3px;border:1px solid #dcdfe6;color:#606266;}"
                                              "QLineEdit:hover{border:1px solid #909399;}"
                                              "QLineEdit:focus{border:1px solid #409EFF;}"));
    m_pEditUser->setStyleSheet(QStringLiteral("QLineEdit{padding-left:25px;padding-right:25px;font-size:20px;font-family:Microsoft Yahei;border-radius:3px;border:1px solid #dcdfe6;color:#606266;}"
                                              "QLineEdit:hover{border:1px solid #909399;}"
                                              "QLineEdit:focus{border:1px solid #409EFF;}"));
    m_pEditPwd->setStyleSheet(QStringLiteral("QLineEdit{padding-left:25px;padding-right:25px;font-size:20px;font-family:Microsoft Yahei;border-radius:3px;border:1px solid #dcdfe6;color:#606266;}"
                                              "QLineEdit:hover{border:1px solid #909399;}"
                                              "QLineEdit:focus{border:1px solid #409EFF;}"));
    m_pBtnSignUp->setStyleSheet(QStringLiteral("QPushButton{font-size:22px;font-family:Microsoft Yahei;color:white;border:none;border-radius:30px;background-color:#409EFF;}"
                                            "QPushButton:hover{background-color:rgb(102,177,255);}"
                                            "QPushButton:pressed{background-color:rgb(58,142,230)};"));

    for(auto & edit : findChildren<QLineEdit*>())
        edit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_pLabelTitle->adjustSize();
    m_pVMainLayout->addStretch();
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pLabelTitle, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pEditNickName, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pEditUser, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pEditPwd, 0, Qt::AlignCenter);
    m_pVMainLayout->addSpacing(40);
    m_pVMainLayout->addWidget(m_pBtnSignUp, 0, Qt::AlignCenter);
    m_pVMainLayout->addStretch();

    connect(m_pBtnSignUp, &QPushButton::clicked, this, &SignUpView::ButtonSignUpClicked);
}

void SignUpView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void SignUpView::ButtonSignUpClicked()
{
    emit Submitted(m_pEditNickName->text(), m_pEditUser->text(), m_pEditPwd->text());
}
