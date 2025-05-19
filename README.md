        administrators.push(_admin1);
        administrators.push(_admin2);

        emit AdminAdded(_admin1);
        emit AdminAdded(_admin2);

        // Даем администраторам и владельцам по 1000 монет
        balances[_admin1] = 1000;
        balances[_admin2] = 1000;
        balances[_owner1] = 1000;
        balances[_owner2] = 1000;

        // Создание недвижимости для первого собственника
        registerProperty(_owner1, 60, "квартира");

        // Создание недвижимости для второго собственника
        registerProperty(_owner2, 87, "квартира");
        registerProperty(_owner2, 250, "дом");
    }

    // Функция 1: Регистрация объекта недвижимости (только для администраторов)
    function registerProperty(address _owner, uint256 _area, string memory _propertyType) public onlyAdmin {
        propertyIdCounter++;
        properties[propertyIdCounter] = Property(_owner, _area, _propertyType);
        emit PropertyRegistered(propertyIdCounter, _owner, _area, _propertyType);
    }

    // Функция 2: Смена роли пользователя (пример - можно реализовать более сложные сценарии)
    function changeRole(address _account, string memory _newRole) public onlyAdmin {
        // В реальном приложении может быть более сложная логика для управления ролями
        emit RoleChanged(_account, _newRole);
    }

    // Функция 3: Создание предложения о продаже недвижимости
    function createSaleOffer(uint256 _propertyId, uint256 _price, uint256 _expiry) public onlyPropertyOwner(_propertyId) {
        require(_expiry > block.timestamp, "Expiry must be in the future.");
        saleOfferIdCounter++;
        saleOffers[saleOfferIdCounter] = SaleOffer(_propertyId, _price, _expiry, msg.sender, address(0), false);
        emit SaleOfferCreated(saleOfferIdCounter, _propertyId, _price, _expiry, msg.sender);
    }

    // Функция 4: Покупка недвижимости (перевод средств)
    function buyProperty(uint256 _saleId) payable public {
        require(saleOffers[_saleId].propertyId > 0, "Sale offer does not exist.");
        require(!saleOffers[_saleId].canceled, "Sale offer is canceled.");
        require(block.timestamp < saleOffers[_saleId].expiry, "Sale offer has expired.");
        require(msg.value >= saleOffers[_saleId].price, "Not enough funds sent."); // >= позволяет купить за большую сумму

        require(saleOffers[_saleId].buyer == address(0), "Property is already bought");

        saleOffers[_saleId].buyer = msg.sender;
    }

    // Функция 5: Подтверждение получения средств продавцом
    function confirmSale(uint256 _saleId) public onlyPropertyOwner(saleOffers[_saleId].propertyId) {
         require(saleOffers[_saleId].propertyId > 0, "Sale offer does not exist.");
        require(!saleOffers[_saleId].canceled, "Sale offer is canceled.");
        require(block.timestamp < saleOffers[_saleId].expiry, "Sale offer has expired.");
        require(saleOffers[_saleId].buyer != address(0), "No buyer yet");

        uint256 propertyId = saleOffers[_saleId].propertyId;
        address seller = properties[propertyId].owner;
        address buyer = saleOffers[_saleId].buyer;
        uint256 price = saleOffers[_saleId].price;


        // Перевод средств продавцу
        payable(seller).transfer(price); // Explicitly cast to payable before transfer
        balances[seller] += price;
        balances[buyer] -= price;


        // Обновление владельца недвижимости
        properties[propertyId].owner = buyer;
        emit PropertySold(propertyId, buyer, seller, buyer, price);

    }


    // Функция 6: Отмена предложения о продаже
    function cancelSaleOffer(uint256 _saleId) public onlyPropertyOwner(saleOffers[_saleId].propertyId) {
        require(saleOffers[_saleId].propertyId > 0, "Sale offer does not exist.");
        require(!saleOffers[_saleId].canceled, "Sale offer is already canceled.");

        saleOffers[_saleId].canceled = true;
        emit SaleOfferCanceled(_saleId);
    }

    // Функция 7: Возврат средств покупателю, если продавец отказался или истек срок
    function refundBuyer(uint256 _saleId) public {
        require(saleOffers[_saleId].buyer == msg.sender, "Only the buyer can request a refund.");
        require(saleOffers[_saleId].propertyId > 0, "Sale offer does not exist.");
        require(saleOffers[_saleId].buyer != address(0), "No buyer");


        require(saleOffers[_saleId].canceled || block.timestamp >= saleOffers[_saleId].expiry, "Sale is still active.");

        address buyer = saleOffers[_saleId].buyer;
        uint256 price = saleOffers[_saleId].price;

        // Возврат средств покупателю
        payable(buyer).transfer(price); // Explicitly cast to payable before transfer
        balances[buyer] += price;
        saleOffers[_saleId].buyer = address(0); // reset buyer

    }


    // Функция 8: Дарение недвижимости (начало процесса)
    function giftProperty(uint256 _propertyId, address _newOwner) public onlyPropertyOwner(_propertyId) {
        require(_newOwner != address(0), "New owner address cannot be zero.");
        //  В реальном приложении можно реализовать более сложную систему с предложением дарения
        Property storage property = properties[_propertyId];  //Using storage
        address oldOwner = property.owner;

        properties[_propertyId].owner = _newOwner;
        emit GiftedProperty(_propertyId, _newOwner, oldOwner);
    }

   // Функция 9: Подтверждение получения дара (новым владельцем)
    function acceptGift(uint256 _propertyId) public {
        require(properties[_propertyId].owner == msg.sender, "You are not the new owner");
        emit GiftAccepted(_propertyId, msg.sender);
    }

     // Функция 10: Получение баланса пользователя
    function getBalance(address _account) public view returns (uint256) {
        return balances[_account];
    }

    // Функция 11: Получение информации об объекте недвижимости
    function getPropertyInfo(uint256 _propertyId) public view returns (address owner, uint256 area, string memory propertyType) {
        Property memory property = properties[_propertyId];  //Using memory
        return (property.owner, property.area, property.propertyType);
    }

    // Функция 12: Удаление администратора
    function removeAdmin(address _adminToRemove) public onlyAdmin {
         require(_adminToRemove != address(0), "Cannot remove zero address as admin");
         require(administrators.length > 1, "Must be at least one admin remaining");

        bool found = false;
        uint256 indexToRemove;

        for (uint256 i = 0; i < administrators.length; i++) {
            if (administrators[i] == _adminToRemove) {
                found = true;
                indexToRemove = i;
                break;
            }
        }

        require(found, "Admin not found");

        // Remove the admin by overwriting with the last element and then popping
        administrators[indexToRemove] = administrators[administrators.length - 1];
        administrators.pop();
    }

    fallback () payable external {
         balances[msg.sender] += msg.value;
    }

    receive() payable external {
         balances[msg.sender] += msg.value;
    }

}

        
