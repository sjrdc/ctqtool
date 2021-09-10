/*
 * this file is part of CTQ tool - a tool to explore critical to quality trees
 * Copyright (C) 2021 Sjoerd Crijns
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "ctqtreescene.h"

#include "datamodel/ctqtree.h"
#include "datamodel/driver.h"
#include "datamodel/ctq.h"

#include <QGraphicsRectItem>
#include <iostream>

namespace CtqTool
{
    CtqTreeScene::CtqTreeScene(QObject* parent) :
        QGraphicsScene(parent)
    {
        CtqTree tree;
        // for (int i = 0; i < 3; ++i)
        // {
        //     UserNeed need;
        //     for (int j = 0; j <= i; ++j)
        //     {
        //     auto driver = std::make_shared<Driver>();
        //     for (int k = 0; k <= j; ++k)
        //     {
        //         auto ctq = std::make_shared<Ctq>();
        //         driver->Add(ctq);
        //     }
        //     need.Add(driver);
        //     }
        //     tree.Add(need);
        // }
        UserNeed need;
        const auto driver = std::make_shared<Driver>();
        const auto driver2 = std::make_shared<Driver>();
        const auto ctq = std::make_shared<Ctq>();
        driver->Add(ctq);
        need.Add(driver);
        need.Add(driver2);
        driver2->Add(ctq);
        driver2->Add(ctq);
        tree.Add(need);
        SetTree(tree);
    }

    void CtqTreeScene::SetTree(const CtqTree& tree)
    {
        {
            auto* item = new QGraphicsRectItem(QRect(-100, -1, 800, 3));
            item->setBrush(QBrush(Qt::yellow));
            this->addItem(item);
        }
        auto ctr = 0ul;
        std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Driver>>> drivers;
        auto column = 10;
        constexpr auto width = 40;
        constexpr auto height = 20;
        constexpr auto offset = height/2;
        static const auto needBrush = QBrush(Qt::red);
        for (auto n = 0; n < tree.GetNrOfNeeds(); ++n)
        {
            const auto need = tree.GetNeed(n);
            const auto breadth = need.GetBreadth();
            const auto nDrivers = need.GetNrOfDrivers();
            const auto row = offset + (ctr - (breadth - 1)*.5)*40;
            std::cout << row << std::endl;
            auto* item = new QGraphicsRectItem(QRect(column, row, width, height));
            item->setBrush(needBrush);
            this->addItem(item);
            ctr += breadth;

            for (auto d = 0; d < nDrivers; ++d)
            drivers.emplace_back(std::make_pair(row, nDrivers), need.GetDriver(d));
        }

        ctr = 0ul;
        column = 80;
        std::vector<std::pair<std::pair<int, int>, std::shared_ptr<Ctq>>> ctqs;
        static const auto driverBrush = QBrush(Qt::blue);
        for (const auto& driver : drivers)
        {
            const auto breadth = driver.second->GetBreadth();
            std::cout << driver.first.first << " " << ctr << " " << breadth << "\n";
            const auto row = driver.first.first + (ctr - ((driver.first.second-1)*.5))*40;
            std::cout << row << std::endl;
            auto* item = new QGraphicsRectItem(QRect(column, row, width, height));
            item->setBrush(driverBrush);
            this->addItem(item);
            ctr += breadth;

            for (auto d = 0; d < driver.second->GetNrOfCtqs(); ++d)
            ctqs.emplace_back(std::make_pair(row, driver.second->GetNrOfCtqs()), driver.second->GetCtq(d));
        }
        
        ctr = 0ul;
        column = 150;    
        static const auto ctqBrush = QBrush(Qt::green);
        for (const auto& ctq : ctqs)
        {
            const auto row = ctq.first.first + (ctr - (ctq.first.second - 1)*.5)*40;
            auto* item = new QGraphicsRectItem(QRect(column, row, width, height));
            this->addItem(item);
            item->setBrush(ctqBrush);
            ctr += 1;
        }
    }
}
