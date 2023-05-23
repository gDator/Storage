#include "ItemDatabase.hpp"

#include "utils.hpp"

void ItemDatabase::initStorage(bool new_database)
{
    if(new_database)
    {
        try
        {
            SQLite::Transaction transaction(m_database);
            m_database.exec("CREATE TABLE IF NOT EXISTS Lager (id INTEGER PRIMARY KEY, Value TEXT, Value2 TEXT, Kategorie TEXT, Package TEXT, Eigenschaften TEXT,Anzahl INTEGER, Hersteller TEXT, Herstellernummer TEXT,\
                            Distributor TEXT, Bestellnummer TEXT, Verpackungseinheit INTEGER, Preis FLOAT, Lagerort TEXT, Datenblatt TEXT, Reserviert INTEGER, Einheit TEXT, Oberkategorie TEXT)");
            m_database.exec("CREATE TABLE IF NOT EXISTS Baugruppen (id_bg INTEGER PRIMARY KEY, Name TEXT)");
            m_database.exec("CREATE TABLE IF NOT EXISTS Link_Lager_Baugruppen (id INTEGER , id_bg INTEGER , Anzahl INTEGER, FOREIGN KEY (id) REFERENCES Lager (id) ON UPDATE CASCADE, FOREIGN KEY (id_bg) REFERENCES Baugruppen (id_bg) ON UPDATE CASCADE)");
            m_database.exec("CREATE TABLE IF NOT EXISTS Reservierung (id INTEGER , id_bg INTEGER , Anzahl INTEGER, FOREIGN KEY (id) REFERENCES Lager (id) ON UPDATE CASCADE, FOREIGN KEY (id_bg) REFERENCES Baugruppen (id_bg) ON UPDATE CASCADE)");
            transaction.commit();
        }
        catch(const std::exception& e)
        {
            cmsg("[error]" + std::string(e.what()));
            LOG_ERROR("Failed initialize");
        }
        m_id = 1;
    }
    else
    {
        SQLite::Statement query(m_database, "SELECT id FROM Lager ORDER BY id DESC");
        int i = 0;
        while (query.executeStep())
        {
            i = std::max(i, query.getColumn(0).getInt());
        }
        m_id = ++i; //next free id
    }
    LOG_HISTORY("ID count" << m_id);
    
}

int ItemDatabase::addItem(Item item)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Lager VALUES(:id, :value, :value2, :cat ,:package, :prop ,:n ,:hrst ,:hrstnr ,:dist ,:bstnr ,:vpe, :price, :storage_place, :datasheet, :reserved, :unit, :maincat)");
        query.bind(":id", m_id);
        query.bind(":value", item.value);
        query.bind(":value2", item.value_2);
        query.bind(":package", item.package);
        query.bind(":cat", item.category);
        query.bind(":prop", item.description);
        query.bind(":n", item.count);
        query.bind(":hrst", item.manufactor);
        query.bind(":hrstnr", item.manufactor_number);
        query.bind(":dist", item.distributor);
        query.bind(":bstnr", item.shop_number);
        query.bind(":vpe", item.vpe);
        query.bind(":price", item.price);
        query.bind(":storage_place", item.storage_place);
        query.bind(":datasheet", item.datasheet);
        query.bind(":reserved", item.reserved);
        query.bind(":unit", item.unit); 
        query.bind(":maincat", item.main_category); 
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": add item:" << item.id << ":" << item.serializeCSV());
    m_id++;
    return (m_id-1);
}

void ItemDatabase::updateItem(Item item)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Lager SET Value = :value, Value2 = :value2, Kategorie = :cat , Package = :package, Eigenschaften = :prop , Anzahl = :n , Hersteller = :hrst , Herstellernummer = :hrstnr , Distributor = :dist , Bestellnummer = :bstnr , Verpackungseinheit = :vpe, Preis = :price , Lagerort = :storage_place, Datenblatt = :datasheet, Reserviert =:reserved, Einheit =:unit, Oberkategorie =:maincat WHERE id = :id");
        query.bind(":id", item.id);
        query.bind(":value", item.value);
        query.bind(":value2", item.value_2);
        query.bind(":cat", item.category);
        query.bind(":package", item.package);
        query.bind(":prop", item.description);
        query.bind(":n", item.count);
        query.bind(":hrst", item.manufactor);
        query.bind(":hrstnr", item.manufactor_number);
        query.bind(":dist", item.distributor);
        query.bind(":bstnr", item.shop_number);
        query.bind(":vpe", item.vpe);
        query.bind(":price", item.price);
        query.bind(":storage_place", item.storage_place);
        query.bind(":datasheet", item.datasheet);
        query.bind(":reserved", item.reserved);
        query.bind(":unit", item.unit);   
        query.bind(":maincat", item.main_category);   
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": updated item:" << item.id << item.serializeCSV());
}

const std::deque<Item>& ItemDatabase::searchItem()
{
    try
    {
        m_list.clear();
        SQLite::Statement query(m_database, "SELECT * FROM Lager");
        
        while (query.executeStep())
        {
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2           = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description        = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved          = query.getColumn(15).getInt();
            res.unit              = query.getColumn(16).getText();
            res.main_category     = query.getColumn(17).getText();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          += getReservationsFromAssembles(res);
            m_list.push_back(res);
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches all items" );
    return m_list;
}

const std::deque<Item>& ItemDatabase::searchItemByID(int id)
{
    try
    {
        m_list.clear();
        SQLite::Statement query(m_database, "SELECT * FROM Lager WHERE id=:id");
        query.bind(":id", id);
        while (query.executeStep())
        {
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2             = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description         = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved          = query.getColumn(15).getInt();
            res.unit              = query.getColumn(16).getText();
            res.main_category     = query.getColumn(17).getText();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          += getReservationsFromAssembles(res);
            m_list.push_back(res);
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches item with id: " << id);
    return m_list;
}

const std::deque<Item>& ItemDatabase::searchItem(Item item)
{
    std::string condition("SELECT * FROM Lager WHERE id > 0");
    try
    {
        
        if(item.main_category.compare("") != 0 && item.main_category.find("Non") == std::string::npos)
        {
           condition += std::string(" AND (");
            std::vector<std::string> filter;
            split(item.main_category, ';', filter);
            for(int j = 0; j < filter.size(); j++)
            {
                condition += std::string(" Oberkategorie LIKE '");
                condition += filter[j];
                condition += std::string("'");
                if(j < filter.size() - 1) //all except last one
                    condition += " OR ";
            }
            condition += ")";
        }
        if(item.value.compare("") != 0)
        {
            condition += std::string(" AND Value LIKE '");
            condition += item.value;
            condition += std::string("'");
        }
        if(item.value_2.compare("") != 0)
        {
            condition += std::string(" AND Value2 LIKE '");
            condition += item.value_2;
            condition += std::string("'");
        }            
        if(item.category.compare("None") != 0)
        {
            condition += std::string(" AND Kategorie LIKE '");
            condition += item.category;
            condition += std::string("'");
        }
        if(item.package.compare("") != 0)
        {
            condition += std::string(" AND Package LIKE '");
            condition += item.package;
            condition += std::string("'");
        }
        if(item.description.compare("") != 0)
        {
            condition += std::string(" AND Eigenschaften LIKE '");
            condition += item.description;
            condition += std::string("'");
        }
        if(item.manufactor.compare("") != 0)
        {
            condition += std::string(" AND Hersteller LIKE '");
            condition += item.manufactor;
            condition += std::string("'");
        }
        if(item.manufactor_number.compare("") != 0)
        {
            condition += std::string(" AND Herstellernummer LIKE '");
            condition += item.manufactor_number;
            condition += std::string("'");
        }
        if(item.distributor.compare("") != 0)
        {
            condition += std::string(" AND Distributor LIKE '");
            condition += item.distributor;
            condition += std::string("'");
        }
        if(item.shop_number.compare("") != 0)
        {
            condition += std::string(" AND Bestellnummer LIKE '");
            condition += item.shop_number;
            condition += std::string("'");
        }
        if(item.vpe > 0)
        {
            condition += std::string(" AND Verpackungseinheit LIKE '");
            condition += item.vpe;
            condition += std::string("'");
        }
        if(item.price > 0)
        {
            condition += std::string(" AND Preis <");
            condition += item.price;
            condition += std::string();
        }
        if(item.storage_place.compare("None|None") != 0)
        {
            condition += std::string(" AND Lagerort LIKE '");
            condition += item.storage_place;
            condition += std::string("'");
        }
        m_list.clear();
        SQLite::Statement query(m_database, condition);
        
        while (query.executeStep())
        {
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2           = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description         = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved            = query.getColumn(15).getInt();
            res.unit                = query.getColumn(16).getText();
            res.main_category       = query.getColumn(17).getText();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          += getReservationsFromAssembles(res);
            m_list.push_back(res);
        }        
    }
    catch(const std::exception& e)
    {
       cmsg("[error]" + std::string(e.what()));
       LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches items:" << condition);
    return m_list;
}

void ItemDatabase::addAssemble(Assemble assamble)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Baugruppen VALUES(NULL, :name)");
        query.bind(":name", assamble.name);
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        (e.what());
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": added assemble: " << assamble.name);
}

bool ItemDatabase::itemExistsInAssemble(Assemble assemble, Item item)
{
    int count = searchItemInAssembleByID(assemble, item).size();
    // Assemble assemble = 
    // for(auto& b : assemble.bom)
    // {
    //     if(std::get<0>(b).id == res.id)
    //         return std::get<1>(b);
    // }
    if(count > 1)
        LOG_ERROR("Multiple existance of same Element");
    if(count == 1)
        return true;
    LOG_HISTORY("Item " << item.id << "does not exist in " << assemble.name);
    return false;
}

const std::deque<Item>& ItemDatabase::searchItemInAssemble(Assemble assemble, Item item)
{
    try
    {
        std::string condition("SELECT Lager.id, Lager.Value, Lager.Value2, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Datenblatt, Lager.Reserviert, Lager.Einheit, Lager.Oberkategorie ");
        condition += "FROM Link_Lager_Baugruppen ";
        condition += "INNER JOIN Lager ON Link_Lager_Baugruppen.id = Lager.id ";
        condition += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        condition += "WHERE Baugruppen.id_bg = :id";

        if(item.value.compare("") != 0)
        {
            condition += std::string(" AND Lager.Value LIKE '");
            condition += item.value;
            condition += std::string("'");
        }
        if(item.value_2.compare("") != 0)
        {
            condition += std::string(" AND Lager.Value2 LIKE '");
            condition += item.value_2;
            condition += std::string("'");
        }
            
        if(item.category.compare("None") != 0)
        {
            condition += std::string(" AND Lager.Kategorie LIKE '");
            condition += item.category;
            condition += std::string("'");
        }
        if(item.package.compare("") != 0)
        {
            condition += std::string(" AND Lager.Package LIKE '");
            condition += item.package;
            condition += std::string("'");
        }
        if(item.description.compare("") != 0)
        {
            condition += std::string(" AND Lager.Eigenschaften LIKE '");
            condition += item.description;
            condition += std::string("'");
        }
        if(item.manufactor.compare("") != 0)
        {
            condition += std::string(" AND Lager.Hersteller LIKE '");
            condition += item.manufactor;
            condition += std::string("'");
        }
        if(item.manufactor_number.compare("") != 0)
        {
            condition += std::string(" AND Lager.Herstellernummer LIKE '");
            condition += item.manufactor_number;
            condition += std::string("'");
        }
        if(item.distributor.compare("") != 0)
        {
            condition += std::string(" AND Lager.Distributor LIKE '");
            condition += item.distributor;
            condition += std::string("'");
        }
        if(item.shop_number.compare("") != 0)
        {
            condition += std::string(" AND Lager.Bestellnummer LIKE '");
            condition += item.shop_number;
            condition += std::string("'");
        }
        if(item.vpe > 0)
        {
            condition += std::string(" AND Lager.Verpackungseinheit LIKE '");
            condition += item.vpe;
            condition += std::string("'");
        }
        if(item.price > 0)
        {
            condition += std::string(" AND Lager.Preis <");
            condition += item.price;
            condition += std::string();
        }
        if(item.storage_place.compare("None|None") != 0)
        {
            condition += std::string(" AND Lager.Lagerort LIKE '");
            condition += item.storage_place;
            condition += std::string("'");
        }

        m_list.clear();
        SQLite::Statement query(m_database, condition);
        query.bind(":id", assemble.id);
        while (query.executeStep())
        {
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2            = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description         = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved          = query.getColumn(15).getInt();
            res.unit              = query.getColumn(16).getText();
            res.main_category       = query.getColumn(17).getText();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          += getReservationsFromAssembles(res);
            m_list.push_back(res);
        }        
    }
    catch(const std::exception& e)
    {
       cmsg("[error]" + std::string(e.what()));
       LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches item: " << item.serializeCSV() << " in assemble " << assemble.name);
    return m_list;
}

const std::deque<Item>& ItemDatabase::searchItemInAssembleByID(Assemble assemble, Item item)
{
    try
    {
        std::string condition("SELECT Lager.id, Lager.Value, Lager.Value2, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Datenblatt, Lager.Reserviert, Lager.Einheit ");
        condition += "FROM Link_Lager_Baugruppen ";
        condition += "INNER JOIN Lager ON Link_Lager_baugruppen.id = Lager.id ";
        condition += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        condition += "WHERE Baugruppen.id_bg = :id_bg AND Lager.id = :id";
        m_list.clear();
        SQLite::Statement query(m_database, condition);
        query.bind(":id_bg", assemble.id);
        query.bind(":id", item.id);
        while (query.executeStep())
        {
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2             = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description         = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved          = query.getColumn(15).getInt();
            res.unit              = query.getColumn(16).getText();
            res.main_category     = query.getColumn(17).getText();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          += getReservationsFromAssembles(res);
            m_list.push_back(res);
        }
    }
    catch(const std::exception& e)
    {
       cmsg("[error]" + std::string(e.what()));
       LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches item with id " << item.id << " in assemble " << assemble.name);
    return m_list;
}

void ItemDatabase::updateItemInAssemble(Assemble assemble, Item item, int count)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Link_Lager_Baugruppen SET Anzahl = :count WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble.id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("[info] Updated Item in Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": updated item: " << item.serializeCSV() << " in assemble " << assemble.name << "to amount of " << count);
}

void ItemDatabase::addItemToAssemble(Assemble assemble, Item item, int count)
{
    //check if exists
    int n = itemExistsInAssemble(assemble, item);
    if(n > 0)
    {
        cmsg("[warning] Bauteil existiert bereits. Anzahl aktualisiert");
        updateItemInAssemble(assemble, item, count);
        return;
    }
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Link_Lager_Baugruppen VALUES( :id, :id_bg, :count)");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble.id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("[info] Added Item to Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": added item: " << item.serializeCSV() << " to assemble " << assemble.name << "with amount" << count);
}

const Assemble ItemDatabase::searchAssemble(Assemble assemble)
{
    m_assemble_list.clear();
    try
    {
        std::string s ("SELECT Lager.id, Lager.Value, Lager.Value2, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Lagerort, Lager.Datenblatt, Lager.Reserviert, Lager.Einheit, Lager.Oberkategorie, Link_Lager_Baugruppen.Anzahl ");
        s += "FROM Link_Lager_Baugruppen ";
        s += "INNER JOIN Lager ON Link_Lager_baugruppen.id = Lager.id ";
        s += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        s += "WHERE Baugruppen.id_bg = :id";
        SQLite::Statement query(m_database, s.c_str());
        query.bind(":id", assemble.id);
        // query.exec();                              
        while (query.executeStep())
        {
            
            Item res;
            res.id                = query.getColumn(0).getInt();
            res.value             = query.getColumn(1).getText();
            res.value_2             = query.getColumn(2).getText();
            res.category          = query.getColumn(3).getText();
            res.package           = query.getColumn(4).getText();
            res.description         = query.getColumn(5).getText();
            res.count             = query.getColumn(6).getInt();
            res.manufactor        = query.getColumn(7).getText();
            res.manufactor_number = query.getColumn(8).getText();
            res.distributor       = query.getColumn(9).getText();
            res.shop_number       = query.getColumn(10).getText();
            res.vpe               = query.getColumn(11).getInt();
            res.price             = query.getColumn(12).getDouble();
            res.storage_place     = query.getColumn(13).getText();
            res.datasheet         = query.getColumn(14).getText();
            // res.reserved          = query.getColumn(15).getInt();
            res.unit              = query.getColumn(16).getText();
            res.main_category       = query.getColumn(17).getText();
            int amount          = query.getColumn(18).getInt();
            res.price_per_unit    = res.price/res.vpe;
            res.reserved          = itemIsReservedFromAssemble(assemble.id, res);
            assemble.bom.push_back(std::make_tuple(res, amount));
        }        
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches bom of assemble" << assemble.name);
    return assemble;
}

const std::deque<Assemble>& ItemDatabase::searchAssembles()
{
    m_assemble_list.clear();
    try
    {
        SQLite::Statement query(m_database, "SELECT * FROM Baugruppen");
        while (query.executeStep())
        {
            Assemble assemble;
            assemble.id               = query.getColumn(0).getInt();
            assemble.name             = query.getColumn(1).getText();
            m_assemble_list.push_back(assemble);
        }        
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": searches assemble list");
    return m_assemble_list;
}

void ItemDatabase::deleteItemFromAssemble(Assemble assemble, Item item)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "DELETE FROM Link_Lager_Baugruppen WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble.id);     
        query.exec();
        transaction.commit();
        cmsg("[info] Deleted Item from Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": deleted item " <<  item.id << "from assemble " << assemble.name);
}

void ItemDatabase::reserveItemToAssemble(int id, Item item, int count, bool stack_reservation)
{
    auto items = searchItemByID(item.id);
    if(items.size() <= 0)
    {
        cmsg("[error] Etwas ist schiefgelaufen");
        return;
    }

    item = items[0]; 
    int n = itemIsReservedFromAssemble(id, item);
    if(n > 0)
    {
        if(stack_reservation)
        {
            cmsg("[warning] Bauteil existiert bereits. Anzahl hinzugefügt");
            if(n+count >= 0)
            {
                updateItemInReservation(id, item, n+count);
            }
            else
                cmsg("[error] Keine negative Reservierung möglich");
            
            return;
        }
        else
        {
            cmsg("[warning] Bauteil existiert bereits. Anzahl aktualisiert");
            updateItemInReservation(id, item, count);
            return;
        }

        
    }
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Reservierung VALUES( :id, :id_bg, :count)");
        query.bind(":id", item.id);
        query.bind(":id_bg", id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        item.count -= count;
        item.reserved = 0;
        updateItem(item);
        cmsg("[info] Added Item to Reservation");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": added item: " << item.serializeCSV() << " to reservation " << id << "with amount" << count);
}

void ItemDatabase::removeReservationFromAssemble(int id, Item item)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "DELETE FROM Reservierung WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", id);     
        query.exec();
        transaction.commit();
        cmsg("[info] Deleted Item from Reservation");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": deleted item " <<  item.id << "from reservation: " << id);
}

void ItemDatabase::removeReservationFromAssemblePartial(int assemble_id, Item item, int count)
{
    int old_count = itemIsReservedFromAssemble(assemble_id, item); 
    count = old_count - count; 
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Reservierung SET Anzahl = :count WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble_id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("[info] Updated Item in Assemble");
        m_updated = true;
        item.reserved = 0;
        updateItem(item);
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": reduced item " <<  item.id << "from reservation with amount " << count);
}

int ItemDatabase::getReservationsFromAssembles(Item item)
{
    int count = 0;
    try
    {
        SQLite::Statement query(m_database, "SELECT Anzahl FROM Reservierung WHERE id=:id");
        query.bind(":id", item.id);
        while (query.executeStep())
        {
            count       += query.getColumn(0).getInt();
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR(e.what());
    }
    // LOG_HISTORY(username << ": reservation count from item with id: " << item.id << " is " << item.reserved);
    return count;
}

int ItemDatabase::itemIsReservedFromAssemble(int assemble_id, Item item)
{
    int n = 0;   
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "SELECT Anzahl FROM Reservierung WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble_id); 
        
        while (query.executeStep())
        {
            n += query.getColumn(0).getInt();
        } 
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    return n;
}

void ItemDatabase::updateItemInReservation(int assemble_id, Item item, int count)
{
    int old_count = itemIsReservedFromAssemble(assemble_id, item); 
    int diff = count - old_count; 
    
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Reservierung SET Anzahl = :count WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", item.id);
        query.bind(":id_bg", assemble_id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("[info] Updated Item in Assemble");
        m_updated = true;
        item.count -= diff; 
        item.reserved = 0;
        updateItem(item);
    }
    catch(const std::exception& e)
    {
        cmsg("[error]" + std::string(e.what()));
        LOG_ERROR(e.what());
    }
    LOG_HISTORY(username << ": updated item: " << item.serializeCSV() << " in reservation " << assemble_id << "to amount of " << count);
}