//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    (c) 2015 Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
//import QtQuick.Extras 1.4

import "./components"

Rectangle {
    // Property Defintions
    id:root

    function activeUasSet() {
        console.log("Vibration Monitor: Active UAS is now set");
        vibrationXIndicator.value = Qt.binding(function() { return vehicleOverview.vibration.x})
        vibrationYIndicator.value = Qt.binding(function() { return vehicleOverview.vibration.y})
        vibrationZIndicator.value = Qt.binding(function() { return vehicleOverview.vibration.z})
    }

    function activeUasUnset() {
        console.log("Vibration Monitor: Active UAS is now unset");
        //Code to make display show a lack of connection here.
    }

//    ProgressBar {
//        width: parent.width
//        height: parent.height
//        value: 50
//        style: ProgressBarStyle {
//            background: Rectangle {
//                radius: 2
//                color: "lightgray"
//                border.color: "gray"
//                border.width: 1
//                implicitWidth: 200
//                implicitHeight: 24
//            }
//            progress: Rectangle {
//                color: "lightsteelblue"
//                border.color: "steelblue"
//            }
//        }
//    }

    Column {
        id: col1
        anchors.centerIn: parent
        width: parent.width
        height: parent.height

        Text {
            id:vibrationXIndicator
            property double value: 0.0
            text: qsTr(" Vib X ") + value
            font.pixelSize: 12
        }

        Text {
            id: vibrationYIndicator
            property double value: 0.0
            text: qsTr(" Vib Y ") + value
            font.pixelSize: 12
        }

        Text {
            id: vibrationZIndicator
            property double value: 0.0
            text: qsTr(" Vib Z ") + value
            font.pixelSize: 12
        }

    }


}

