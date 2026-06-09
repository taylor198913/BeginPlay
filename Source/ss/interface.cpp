
using namespace std;

// 物品， 
class Item {
    // 物品名称
    string name;

    // 耐久 - 
    int durability;

    uint maxSize = 60; // 最大的堆叠数量
}

// 60

// 子弹 1， 60发 - m7


// awm - 20发 

class Gezi {
    // 某种物品
    vector<Item> itemVec; // 数组


    // 放东西
    // Item,
    push(Item item) bool {
        if (this.itemVec.size() == 0) {
            this.itemVec.push(item);
            return true;
        }

        // 
        auto f_item = this.itemVec.front()
        if (f_item.name == item.name) {
            auto 
        }

        this.itemVec.push(item);
    }

    // 取东西
    pop()  {
        auto item = this.itemVec.back();
        this.itemVec.pop_back();
        return
    }
}


class BeiBao {
    Gezi[] map;

    // 放物品，有重复的就堆叠
    addItem() void;

    delItem() void;

    // 获取背包中的物品列表
    getItemList() void;
}
