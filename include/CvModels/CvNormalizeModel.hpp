#pragma once

#include <iostream>

#include <QDoubleValidator>
#include <QtCore/QObject>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/NodeDelegateModelRegistry>

#include "DataTypes/ImageData.hpp"
#include "Utils/Utils.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class CvNormalizeModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    CvNormalizeModel();

    ~CvNormalizeModel() override{delete _box;std::cout<<"delete CvNormalizeModel"<<std::endl;};

public:
    QString caption() const override { return QString("Normalize"); }

    QString name() const override { return QString("Normalize:正则化"); }

public:
    virtual QString modelName() const { return QString("Normalize"); }

    unsigned int nPorts(PortType const portType) const override;

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex const portIndex) override;

    QWidget *embeddedWidget() override { return _box; };

    bool resizable() const override { return true; }

    void compute();

    void load(const QJsonObject &) override;

    QJsonObject save() const override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    QGroupBox *_box;
    QLabel *_label;
    QLineEdit *minv;
    QLineEdit *maxv;

    cv::Mat _mat;
    QPixmap _q_pix;
    std::shared_ptr<NodeData> _nodeData;
};
