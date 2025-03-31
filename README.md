// SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.9.0;

import "./SafeMath.sol";

contract BankDeposit {
    using SafeMath for uint256; // Изменено на uint256

    mapping(address => uint256) public userDeposit;
    mapping(address => uint256) public balance;
    mapping(address => uint256) public time;
    mapping(address => uint256) public percentWithdraw;
    mapping(address => uint256) public allPercentWithdraw;
    uint256 public stepTime = 0.05 hours;

    event Invest(address indexed investor, uint256 amount);
    event Withdraw(address indexed investor, uint256 amount);

    modifier userExist() {
        require(balance[msg.sender] > 0, "nope clients");
        _;
    }

    modifier checkTime() {
        require(now >= time[msg.sender].add(stepTime), "more fast zapros money");
        _;
    }

    function bankAccount() public payable {
        require(msg.value >= .001 ether);
    }

    function collectPercent() userExist checkTime public {
        if ((balance[msg.sender].mul(2)) <= allPercentWithdraw[msg.sender]) {
            balance[msg.sender] = 0;
            time[msg.sender] = 0;
            percentWithdraw[msg.sender] = 0;
        } else {
            uint256 payout = payableAmount(); 
            percentWithdraw[msg.sender] = percentWithdraw[msg.sender].add(payout);
            allPercentWithdraw[msg.sender] = allPercentWithdraw[msg.sender].add(payout);
            payable(msg.sender).transfer(payout); // Изменено на payable(msg.sender)
            emit Withdraw(msg.sender, payout);
        }
    }

    function deposit() public payable {
        if (msg.value > 0) {
            if (balance[msg.sender] > 0 && now > time[msg.sender].add(stepTime)) {
                collectPercent();
                percentWithdraw[msg.sender] = 0;
            }
            balance[msg.sender] = balance[msg.sender].add(msg.value);
            time[msg.sender] = now;
            emit Invest(msg.sender, msg.value);
        }
    }

    function percentRate() public view returns (uint256) {
        if (balance[msg.sender] < 10 ether) {
            return 5;
        }
        if (balance[msg.sender] >= 10 ether && balance[msg.sender] < 20 ether) {
            return 7;
        }
        if (balance[msg.sender] >= 20 ether && balance[msg.sender] < 30 ether) {
            return 8;
        }
        if (balance[msg.sender] >= 30 ether) {
            return 9;
        }
        return 0; // Добавлено для обработки случая, когда ни одно условие не выполняется
    }

    function payableAmount() public view returns (uint256) {
        uint256 percent = percentRate();
        uint256 different = now.sub(time[msg.sender]).div(stepTime);
        uint256 rate = balance[msg.sender].div(100).mul(percent); // Исправлено на mul
        uint256 withdrawalAmount = rate.mul(different).sub(percentWithdraw[msg.sender]);
        return withdrawalAmount;
    }

    function returnDeposit() public {
        uint256 withdrawalAmount = balance[msg.sender];
        balance[msg.sender] = 0;
        time[msg.sender] = 0;
        percentWithdraw[msg.sender] = 0;
        payable(msg.sender).transfer(withdrawalAmount); // Изменено на payable(msg.sender)
    }
}
