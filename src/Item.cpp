#include "Item.hpp"
#define DEBUG
#include "logger.hpp"
void ItemDatabase::initStorage(bool new_database)
{
    if(new_database)
    {
        try
        {
            SQLite::Transaction transaction(m_database);
            m_database.exec("CREATE TABLE IF NOT EXISTS Lager (id INTEGER PRIMARY KEY, Value TEXT, Kategorie TEXT, Package TEXT, Eigenschaften TEXT,Anzahl INTEGER, Hersteller TEXT, Herstellernummer TEXT,\
                            Distributor TEXT, Bestellnummer TEXT, Verpackungseinheit TEXT, Preis FLOAT, Lagerort TEXT, Datenblatt TEXT)");
            m_database.exec("CREATE TABLE IF NOT EXISTS Baugruppen (id_bg INTEGER PRIMARY KEY, Name TEXT)");
            m_database.exec("CREATE TABLE IF NOT EXISTS Link_Lager_Baugruppen (id INTEGER , id_bg INTEGER , Anzahl INTEGER, FOREIGN KEY (id) REFERENCES Lager (id) ON UPDATE CASCADE, FOREIGN KEY (id_bg) REFERENCES Baugruppen (id_bg) ON UPDATE CASCADE)");
            transaction.commit();
        }
        catch(const std::exception& e)
        {
            cmsg(e.what());
            LOG_ERROR("Failed initialize");
        }
        id = 1;
    }
    else
    {
        SQLite::Statement query(m_database, "SELECT id FROM Lager ORDER BY id DESC");
        int i = 0;
        while (query.executeStep())
        {
            i = std::max(i, query.getColumn(0).getInt());
        }
        id = ++i; //next free id
    }
    LOG_TRACE(id);
    
}

int ItemDatabase::addItem(Item item)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Lager VALUES(:id, :value, :cat ,:package, :prop ,:n ,:hrst ,:hrstnr ,:dist ,:bstnr ,:vpe, :price, :storage_place, :datasheet)");
        query.bind(":id", id);
        query.bind(":value", item.value);
        query.bind(":package", item.package);
        query.bind(":cat", item.category);
        query.bind(":prop", item.properties);
        query.bind(":n", item.count);
        query.bind(":hrst", item.manufactor);
        query.bind(":hrstnr", item.manufactor_number);
        query.bind(":dist", item.distributor);
        query.bind(":bstnr", item.shop_number);
        query.bind(":vpe", item.vpe);
        query.bind(":price", item.price);
        query.bind(":storage_place", item.storage_place);
        query.bind(":datasheet", item.datasheet);        
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
    id++;
    return (id-1);
}


void ItemDatabase::updateItem(Item item)
{
    try
    {
        LOG_TRACE("Update Item " << item.id << ": " << item.count);
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Lager SET Value = :value, Kategorie = :cat , Package = :package, Eigenschaften = :prop , Anzahl = :n , Hersteller = :hrst , Herstellernummer = :hrstnr , Distributor = :dist , Bestellnummer = :bstnr , Verpackungseinheit = :vpe, Preis = :price , Lagerort = :storage_place, Datenblatt = :datasheet WHERE id = :id");
        query.bind(":id", item.id);
        query.bind(":value", item.value);
        query.bind(":cat", item.category);
        query.bind(":package", item.package);
        query.bind(":prop", item.properties);
        query.bind(":n", item.count);
        query.bind(":hrst", item.manufactor);
        query.bind(":hrstnr", item.manufactor_number);
        query.bind(":dist", item.distributor);
        query.bind(":bstnr", item.shop_number);
        query.bind(":vpe", item.vpe);
        query.bind(":price", item.price);
        query.bind(":storage_place", item.storage_place);
        query.bind(":datasheet", item.datasheet);        
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
}

const std::deque<Item>& ItemDatabase::searchItem()
{
    try
    {
        m_list.clear();
        SQLite::Statement query(m_database, "SELECT * FROM Lager");
        
        while (query.executeStep())
        {
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            m_list.push_back(i);
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR(e.what());
    }
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
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            m_list.push_back(i);
        }        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR(e.what());
    }
    return m_list;
}

const std::deque<Item>& ItemDatabase::searchItem(Item i)
{
    try
    {
        std::string condition("SELECT * FROM Lager WHERE id > 0");
        if(i.value.compare("") != 0)
        {
            condition += std::string(" AND Value LIKE '");
            condition += i.value;
            condition += std::string("'");
        }
            
        if(i.category.compare("None") != 0)
        {
            condition += std::string(" AND Kategorie LIKE '");
            condition += i.category;
            condition += std::string("'");
        }
        if(i.package.compare("") != 0)
        {
            condition += std::string(" AND Package LIKE '");
            condition += i.package;
            condition += std::string("'");
        }
        if(i.properties.compare("") != 0)
        {
            condition += std::string(" AND Eigenschaften LIKE '");
            condition += i.properties;
            condition += std::string("'");
        }
        if(i.manufactor.compare("") != 0)
        {
            condition += std::string(" AND Hersteller LIKE '");
            condition += i.manufactor;
            condition += std::string("'");
        }
        if(i.manufactor_number.compare("") != 0)
        {
            condition += std::string(" AND Herstellernummer LIKE '");
            condition += i.manufactor_number;
            condition += std::string("'");
        }
        if(i.distributor.compare("") != 0)
        {
            condition += std::string(" AND Distributor LIKE '");
            condition += i.distributor;
            condition += std::string("'");
        }
        if(i.shop_number.compare("") != 0)
        {
            condition += std::string(" AND Bestellnummer LIKE '");
            condition += i.shop_number;
            condition += std::string("'");
        }
        if(i.vpe.compare("") != 0)
        {
            condition += std::string(" AND Verpackungseinheit LIKE '");
            condition += i.vpe;
            condition += std::string("'");
        }
        if(i.price > 0)
        {
            condition += std::string(" AND Preis <");
            condition += i.price;
            condition += std::string();
        }
        if(i.storage_place.compare("None|None") != 0)
        {
            condition += std::string(" AND Lagerort LIKE '");
            condition += i.storage_place;
            condition += std::string("'");
        }

        m_list.clear();
        SQLite::Statement query(m_database, condition);
        
        while (query.executeStep())
        {
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            m_list.push_back(i);
        }        
    }
    catch(const std::exception& e)
    {
       cmsg(e.what());
       LOG_ERROR(e.what());
    }
    return m_list;
}

void ItemDatabase::addAssemble(Assemble a)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Baugruppen VALUES(NULL, :name)");
        query.bind(":name", a.name);
        query.exec();
        transaction.commit();
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
}

int ItemDatabase::itemExistsInAssemble(Assemble a, Item i)
{
    int count = searchItemInAssembleByID(a, i).size();
    // Assemble assemble = 
    // for(auto& b : assemble.bom)
    // {
    //     if(std::get<0>(b).id == i.id)
    //         return std::get<1>(b);
    // }
    if(count > 1)
        LOG_ERROR("Multiple existance of same Element");
    if(count == 1)
        return 1;
    LOG_INFO("Item " << i.id << "does not exist in " << a.name);
    return -1;
}

const std::deque<Item>& ItemDatabase::searchItemInAssemble(Assemble a, Item i)
{
    try
    {
        std::string condition("SELECT Lager.id, Lager.Value, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Datenblatt, Link_Lager_Baugruppen.Anzahl ");
        condition += "FROM Link_Lager_Baugruppen ";
        condition += "INNER JOIN Lager ON Link_Lager_baugruppen.id = Lager.id ";
        condition += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        condition += "WHERE Baugruppen.id_bg = :id";

        if(i.value.compare("") != 0)
        {
            condition += std::string(" AND Lager.Value LIKE '");
            condition += i.value;
            condition += std::string("'");
        }
            
        if(i.category.compare("None") != 0)
        {
            condition += std::string(" AND Lager.Kategorie LIKE '");
            condition += i.category;
            condition += std::string("'");
        }
        if(i.package.compare("") != 0)
        {
            condition += std::string(" AND Lager.Package LIKE '");
            condition += i.package;
            condition += std::string("'");
        }
        if(i.properties.compare("") != 0)
        {
            condition += std::string(" AND Lager.Eigenschaften LIKE '");
            condition += i.properties;
            condition += std::string("'");
        }
        if(i.manufactor.compare("") != 0)
        {
            condition += std::string(" AND Lager.Hersteller LIKE '");
            condition += i.manufactor;
            condition += std::string("'");
        }
        if(i.manufactor_number.compare("") != 0)
        {
            condition += std::string(" AND Lager.Herstellernummer LIKE '");
            condition += i.manufactor_number;
            condition += std::string("'");
        }
        if(i.distributor.compare("") != 0)
        {
            condition += std::string(" AND Lager.Distributor LIKE '");
            condition += i.distributor;
            condition += std::string("'");
        }
        if(i.shop_number.compare("") != 0)
        {
            condition += std::string(" AND Lager.Bestellnummer LIKE '");
            condition += i.shop_number;
            condition += std::string("'");
        }
        if(i.vpe.compare("") != 0)
        {
            condition += std::string(" AND Lager.Verpackungseinheit LIKE '");
            condition += i.vpe;
            condition += std::string("'");
        }
        if(i.price > 0)
        {
            condition += std::string(" AND Lager.Preis <");
            condition += i.price;
            condition += std::string();
        }
        if(i.storage_place.compare("None|None") != 0)
        {
            condition += std::string(" AND Lager.Lagerort LIKE '");
            condition += i.storage_place;
            condition += std::string("'");
        }

        m_list.clear();
        SQLite::Statement query(m_database, condition);
        query.bind(":id", a.id);
        while (query.executeStep())
        {
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            m_list.push_back(i);
        }        
    }
    catch(const std::exception& e)
    {
       cmsg(e.what());
       LOG_ERROR(e.what());
    }
    return m_list;
}

const std::deque<Item>& ItemDatabase::searchItemInAssembleByID(Assemble a, Item i)
{
    try
    {
        std::string condition("SELECT Lager.id, Lager.Value, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Datenblatt, Link_Lager_Baugruppen.Anzahl ");
        condition += "FROM Link_Lager_Baugruppen ";
        condition += "INNER JOIN Lager ON Link_Lager_baugruppen.id = Lager.id ";
        condition += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        condition += "WHERE Baugruppen.id_bg = :id_bg AND Lager.id = :id";
        m_list.clear();
        SQLite::Statement query(m_database, condition);
        query.bind(":id_bg", a.id);
        query.bind(":id", i.id);
        while (query.executeStep())
        {
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            m_list.push_back(i);
        }
    }
    catch(const std::exception& e)
    {
       cmsg(e.what());
       LOG_ERROR(e.what());
    }

    return m_list;
}

void ItemDatabase::updateItemInAssemble(Assemble a, Item i, int count)
{
    try
    {
        LOG_INFO("Try Update Item " << i.id << "to: " << count << " in Assembly: " << a.name);
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "UPDATE Link_Lager_Baugruppen SET Anzahl = :count WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", i.id);
        query.bind(":id_bg", a.id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("Updated Item in Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
}

void ItemDatabase::addItemToAssemble(Assemble a, Item i, int count)
{
    //check if exists
    int n = itemExistsInAssemble(a, i);
    if(n > 0)
    {
        cmsg("[warning] Bauteil existiert bereits. Anzahl aktualisiert");
        updateItemInAssemble(a, i, count);
        return;
    }
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "INSERT INTO Link_Lager_Baugruppen VALUES( :id, :id_bg, :count)");
        query.bind(":id", i.id);
        query.bind(":id_bg", a.id);     
        query.bind(":count", count);     
        query.exec();
        transaction.commit();
        cmsg("Added Item to Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
}


const Assemble ItemDatabase::searchAssemble(Assemble a)
{
    LOG_TRACE("");
    m_assemble_list.clear();
    try
    {
        std::string s ("SELECT Lager.id, Lager.Value, Lager.Kategorie, Lager.Package, Lager.Eigenschaften, Lager.Anzahl, Lager.Hersteller, Lager.Herstellernummer, Lager.Distributor, Lager.Bestellnummer, Lager.Verpackungseinheit, Lager.Preis, Lager.Lagerort, Lager.Datenblatt, Link_Lager_Baugruppen.Anzahl ");
        s += "FROM Link_Lager_Baugruppen ";
        s += "INNER JOIN Lager ON Link_Lager_baugruppen.id = Lager.id ";
        s += "INNER JOIN Baugruppen ON Baugruppen.id_bg = Link_Lager_Baugruppen.id_bg ";
        s += "WHERE Baugruppen.id_bg = :id";
        SQLite::Statement query(m_database, s.c_str());
        query.bind(":id", a.id);
        // query.exec();                              
        while (query.executeStep())
        {
            
            Item i;
            i.id                = query.getColumn(0).getInt();
            i.value             = query.getColumn(1).getText();
            i.category          = query.getColumn(2).getText();
            i.package           = query.getColumn(3).getText();
            i.properties         = query.getColumn(4).getText();
            i.count             = query.getColumn(5).getInt();
            i.manufactor        = query.getColumn(6).getText();
            i.manufactor_number = query.getColumn(7).getText();
            i.distributor       = query.getColumn(8).getText();
            i.shop_number       = query.getColumn(9).getText();
            i.vpe               = query.getColumn(10).getText();
            i.price             = query.getColumn(11).getDouble();
            i.storage_place     = query.getColumn(12).getText();
            i.datasheet         = query.getColumn(13).getText();
            int amount          = query.getColumn(14).getInt();

            a.bom.push_back(std::make_tuple(i, amount));
            LOG_TRACE("Found " << i.id << " in " << a.name);
        }        
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
    return a;
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
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
    //TODO: item fill data 
    return m_assemble_list;
}

void ItemDatabase::deleteItemFromAssemble(Assemble a, Item i)
{
    try
    {
        SQLite::Transaction transaction(m_database);
        SQLite::Statement query(m_database, "DELETE FROM Link_Lager_Baugruppen WHERE id = :id AND id_bg = :id_bg");
        query.bind(":id", i.id);
        query.bind(":id_bg", a.id);     
        query.exec();
        transaction.commit();
        cmsg("Deleted Item from Assemble");
        m_updated = true;
    }
    catch(const std::exception& e)
    {
        cmsg(e.what());
        LOG_ERROR(e.what());
    }
}
