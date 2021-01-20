//
// Created by TAADEJOM on 18.01.2021.
//

#ifndef QDATAVIS_SETTINGMANAGER_H
#define QDATAVIS_SETTINGMANAGER_H

struct SettingManager {

    static inline QVariant getSetting(const QString &setting) {
        QSettings settings;
        return settings.value(setting);
    }

};

#endif //QDATAVIS_SETTINGMANAGER_H
