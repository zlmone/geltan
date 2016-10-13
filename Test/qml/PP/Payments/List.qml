import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import Buschtrommel.Geltan.PayPal 1.0
import "../../common"

ScrollView {
    id: ppPaymentsListView
    objectName: "listPayPalPaymentsView"
    property int margin: 10
    width: parent.width
    height: parent.height

    property string pageTitle: "List PayPal Payments"

    Flickable {
        anchors.fill: parent
        contentHeight: col.visible ? col.height + margin : height

        PPListPayments {
            id: ppListPayments
            token: config.payPalToken
            tokenType: config.payPalTokenType
        }

        BusyIndicator {
            anchors.centerIn: parent
            running: ppListPayments.inOperation
        }

        ColumnLayout {
            id: col
            anchors { left: parent.left; right: parent.right; top: parent.top; margins: margin }
            visible: !ppListPayments.inOperation

            LabeledTextField {
                id: countField
                label: "Count"
                text: ppListPayments.count
                inputMethodHints: Qt.ImhDigitsOnly
                onTextChanged: if (text.length > 0) { ppListPayments.count = parseInt(text) } else { ppListPayments.count = 0 }
                visible: !ppListPayments.paymentList
            }

            LabeledTextField {
                id: startIdField
                label: "Start ID"
                onTextChanged: ppListPayments.startId = text
                visible: !ppListPayments.paymentList
            }

            LabeledTextField {
                id: startIndexField
                label: "Start Index"
                text: ppListPayments.startIndex
                inputMethodHints: Qt.ImhDigitsOnly
                onTextChanged: if (text.length > 0) { ppListPayments.startIndex = parseInt(text) } else { ppListPayments.startIndex = 0 }
                visible: !ppListPayments.paymentList
            }

            Label { text: "Start Time"; visible: !ppListPayments.paymentList }

            Calendar {
                Layout.fillWidth: true
                onClicked: ppListPayments.startTime = date
                visible: !ppListPayments.paymentList
            }

            Label { text: "End Time"; }

            Calendar {
                Layout.fillWidth: true
                onClicked: ppListPayments.endTime = date
                visible: !ppListPayments.paymentList
            }

            LabeledTextField {
                id: sortByField
                label: "Sort By"
                text: ppListPayments.sortBy
                onTextChanged: ppListPayments.sortBy = text
                visible: !ppListPayments.paymentList
            }

            Label { text: "Sort Order"; visible: !ppListPayments.paymentList }

            ComboBox {
                Layout.fillWidth: true
                textRole: "text"
                model: ListModel {
                    id: sortOrderModel
                    ListElement { text: "Ascending"; value: Qt.AscendingOrder }
                    ListElement { text: "Descending"; value: Qt.DescendingOrder }
                }
                onCurrentIndexChanged: ppListPayments.sortOrder = sortOrderModel.get(currentIndex).value
                visible: !ppListPayments.paymentList
            }

            Button {
                text: "Load Payments"
                Layout.alignment: Qt.AlignHCenter
                onClicked: ppListPayments.call()
                visible: !ppListPayments.paymentList
            }

            Repeater {
                model: ppListPayments.paymentList
                Item {
                    width: parent.width
                    height: itemCol.height + 5

                    ColumnLayout {
                        id: itemCol
                    }
                }
            }
        }
    }
}
