#include "../include/CvModels/CvNormalizeModel.hpp"

#include "../include/DataTypes/ImageData.hpp"

#include "Widget/Full2DDialog.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QtWidgets/QFileDialog>

CvNormalizeModel::CvNormalizeModel()
    : _box(new QGroupBox())
    , _label(new QLabel("Image Visual", _box))
{
    auto full_lay = new QVBoxLayout(_box);
    auto f = _box->font();
    f.setBold(true);
    _box->setFont(f);

    _label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    _label->setMinimumSize(200, 200);
    _label->installEventFilter(this);

    auto group = new QGroupBox("范围", _box);
    auto lay = new QHBoxLayout(_box);
    minv = new QLineEdit("0");
    maxv = new QLineEdit("1");
    auto label = new QLabel("~", _box);
    lay->addWidget(minv);
    lay->addWidget(label);
    lay->addWidget(maxv);
    group->setLayout(lay);

    full_lay->addWidget(_label);
    full_lay->addWidget(group);
    _box->setLayout(full_lay);
    _box->resize(200, 200);
}

unsigned int CvNormalizeModel::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType) {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 1;

    default:;
        break;
    };

    return result;
}

bool CvNormalizeModel::eventFilter(QObject *object, QEvent *event)
{
    if (object == _label) {
        int w = _label->width();
        int h = _label->height();

        if (event->type() == QEvent::Resize) {
            auto d = std::dynamic_pointer_cast<ImageData>(_nodeData);
            if (d) {
                if (_mat.empty()) {
                    return false;
                };
                auto pix = QPixmap::fromImage(cvMat2QImage(_mat));
                _label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
            }
        } else if (event->type() == QEvent::MouseButtonPress) {
            if (_mat.empty()) {
                return false;
            }
            auto shower = new Full2DDialog(nullptr, &_mat);
            shower->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
            shower->showNormal();
            shower->exec();
            shower->deleteLater();
        }
    }
    return false;
}

NodeDataType CvNormalizeModel::dataType(PortType const, PortIndex const) const
{
    return ImageData().type();
}

std::shared_ptr<NodeData> CvNormalizeModel::outData(PortIndex)
{
    return std::make_shared<ImageData>(_mat);
}

void CvNormalizeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    _nodeData = nodeData;

    if (_nodeData) {
        auto d = std::dynamic_pointer_cast<ImageData>(_nodeData);
        if (d->mat().empty()) {
            return;
        };

        this->compute();

    } else {
        _label->setPixmap(QPixmap());
    }

    // dataUpdated 触发下游的节点更新
    //    Q_EMIT dataUpdated(0);
}
void CvNormalizeModel::compute()
{
    auto d = std::dynamic_pointer_cast<ImageData>(_nodeData);
    if (!d) {
        return;
    }
    if (d->mat().empty()) {
        return;
    }
    bool f;
    auto min_val = minv->text().toFloat(&f);
    auto max_val = maxv->text().toFloat(&f);
    if (!f) {
        return;
    }
    int w = _label->width();
    int h = _label->height();

    cv::normalize(d->mat(),_mat,min_val,max_val,cv::NORM_MINMAX);
    _mat.convertTo(_mat, CV_8UC1);//
//    cv::imwrite("")
    auto pix = QPixmap::fromImage(cvMat2QImage(_mat));
    _label->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    Q_EMIT dataUpdated(0);
}
void CvNormalizeModel::load(const QJsonObject &s)
{
    minv->setText(s["minv"].toString());
    maxv->setText(s["maxv"].toString());
}

QJsonObject CvNormalizeModel::save() const
{
    auto s = NodeDelegateModel::save();
    s["minv"] = minv->text();
    s["maxv"] = maxv->text();

    return s;
}
