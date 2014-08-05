#include "data/CClusterDefinition.h"
#include "CClusterEditorDialog.h"
#include "CMainWindow.h"
#include "CClusterEditorAddDialog.h"
#include "ui_CClusterEditorDialog.h"
#include <QMessageBox>
#include <QPushButton>

CClusterEditorDialog::CClusterEditorDialog(QWidget *parent, CSelectionData *data, ClusterMap *clusters, QString clusterName) :
    QDialog(parent),
    ui(new Ui::CClusterEditorDialog),
    m_data(data), m_clusterMap(clusters)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->installEventFilter(this);
    m_testCases = new QStandardItemModel(ui->listViewTests);
    ui->listViewTests->setModel(m_testCases);
    m_codeElements = new QStandardItemModel(ui->listViewCEs);
    ui->listViewCEs->setModel(m_codeElements);

    fillListViews(clusterName);
}

CClusterEditorDialog::~CClusterEditorDialog()
{
    delete ui;
}

bool CClusterEditorDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->buttonBox->button(QDialogButtonBox::Ok) && event->type() == QEvent::MouseButtonRelease) {
        if (ui->lineEditClusterName->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing cluster name.");
            return true;
        }
    }
    return false;
}

void CClusterEditorDialog::on_toolButtonAddTests_clicked()
{
    CClusterEditorAddDialog dia(this, m_data->getTestcases(), m_testCases);
    dia.setWindowTitle("Add test cases");
    dia.exec();
}

void CClusterEditorDialog::on_toolButtonRemoveTests_clicked()
{
    QList<QPersistentModelIndex> indexes;
    foreach (const QModelIndex &i, ui->listViewTests->selectionModel()->selectedIndexes())
        indexes << i;

    foreach (const QPersistentModelIndex &i, indexes)
        m_testCases->removeRow(i.row());
}

void CClusterEditorDialog::on_toolButtonAddCEs_clicked()
{
    CClusterEditorAddDialog dia(this, m_data->getCodeElements(), m_codeElements, true);
    dia.setWindowTitle("Add code elements");
    dia.exec();
}

void CClusterEditorDialog::on_toolButtonRemoveCEs_clicked()
{
    QList<QPersistentModelIndex> indexes;
    foreach (const QModelIndex &i, ui->listViewCEs->selectionModel()->selectedIndexes())
        indexes << i;

    foreach (const QPersistentModelIndex &i, indexes)
        m_codeElements->removeRow(i.row());
}

void CClusterEditorDialog::on_buttonBox_accepted()
{
    String clusterName = ui->lineEditClusterName->text().toStdString();
    (*m_clusterMap)[clusterName] = createCluster();
    qobject_cast<CMainWindow*>(parent())->updateAvailableClusters();
}

void CClusterEditorDialog::addTestCases(QStringList list)
{
    for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
        QStandardItem *item = new QStandardItem(*it);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_testCases->appendRow(item);
    }
}

void CClusterEditorDialog::addCodeElements(QStringList list)
{
    for (QStringList::iterator it = list.begin(); it != list.end(); ++it) {
        QStandardItem *item = new QStandardItem(*it);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        m_codeElements->appendRow(item);
    }
}

CClusterDefinition CClusterEditorDialog::createCluster()
{
    CClusterDefinition cluster;
    // add testcases
    for (int i = 0; i < m_testCases->rowCount(); ++i) {
        String test = m_testCases->item(i)->text().toStdString();
        cluster.addTestCase(m_data->getTestcases()->getID(test));
    }
    // add code elements
    for (int i = 0; i < m_codeElements->rowCount(); ++i) {
        String codeElement = m_codeElements->item(i)->text().toStdString();
        cluster.addCodeElement(m_data->getCodeElements()->getID(codeElement));
    }

    return cluster;
}

void CClusterEditorDialog::fillListViews(QString clusterName)
{
    if (clusterName.isEmpty())
        return;

    bool isTestSuiteAvailable = qobject_cast<CMainWindow*>(parent())->isTestSuiteAvailable();
    ui->lineEditClusterName->setText(clusterName);

    CClusterDefinition cluster = (*m_clusterMap)[clusterName.toStdString()];
    IntVector idVector;
    QStringList idNameList;

    idVector = cluster.getTestCases();
    for (IntVector::const_iterator it = idVector.begin(); it != idVector.end(); ++it) {
        if (isTestSuiteAvailable)
            idNameList << m_data->getTestcases()->getValue((*it)).c_str();
        else
            idNameList << QString::number(*it);
    }
    addTestCases(idNameList);

    idVector.clear();
    idNameList.clear();

    idVector = cluster.getCodeElements();
    for (IntVector::const_iterator it = idVector.begin(); it != idVector.end(); ++it) {
        if (isTestSuiteAvailable)
            idNameList << m_data->getCodeElements()->getValue((*it)).c_str();
        else
            idNameList << QString::number(*it);
    }
    addCodeElements(idNameList);
}
