исправь ошибку
/C/Users/student/pz17-22 yes/contracts/BankDeposit.sol:6:9: TypeError: Library name expected.
  using SafeMath for uint;

  код bankDeposit
  // SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.9.0;

import "./SafeMath.sol";
contract BankDeposit {
  using SafeMath for uint;
  mapping(address => uint) public userDeposit;
  mapping(address => uint) public balance;
  mapping(address => uint) public time;
  mapping(address => uint) public percentWithdraw;
  mapping(address => uint) public allPercentWithdraw;
  uint public stepTime = 0.05 hours;

  event Invest(address investor, uint256 amount);
  event Withdraw(address investor, uint256 amount);

  modifier userExist() {
  require(balance[msg.sender] > 0, "nope clients");
  _;
  }

  modifier checkTime() {
  require(now >= time[msg.sender].add(stepTime), "more fast zapros money");
  _;
  }

  function bankAccount()public payable{
    require(msg.value >= .001 ether);
  }

  function collectPercent() userExist checkTime public {
    if ((balance[msg.sender].mul(2)) <= allPercentWithdraw[msg.sender]) {
        balance[msg.sender] = 0;
        time[msg.sender] = 0;
        percentWithdraw[msg.sender] = 0;
    } else {
        uint payout = payableAmount(); 
        percentWithdraw[msg.sender] = percentWithdraw[msg.sender].add(payout);
        allPercentWithdraw[msg.sender] = allPercentWithdraw[msg.sender].add(payout);
        msg.sender.transfer(payout);
        emit Withdraw(msg.sender, payout);
    }
  }

  function deposit() public payable{
    if(msg.value > 0){
      if(balance[msg.sender] > 0 && now > time[msg.sender].add(stepTime)){
        collectPercent();
        percentWithdraw[msg.sender] = 0;
      }
      balance[msg.sender] = balance[msg.sender].add(msg.value);
      time[msg.sender] = now;
      emit Invest(msg.sender, msg.value);
    }
  }

  function percentRate() public view returns(uint){
    if(balance[msg.sender] < 10 ether){
      return (5);
    }
    if(balance[msg.sender] >= 10 ether && balance[msg.sender] < 20 ether){
      return (7);
    }
    if(balance[msg.sender] >= 20 ether && balance[msg.sender] < 30 ether){
      return (8);
    }
    if(balance[msg.sender] >= 30 ether ){
      return (9);
    }
  }

  function payableAmount() public view returns(uint256){
    uint percent = percentRate();
    uint different = now.sub(time[msg.sender]).div(stepTime);
    uint rate = balance[msg.sender].div(100).mull(percent);
    uint withdrawalAmount = rate.mul(different).sub(percentWithdraw[msg.sender]);
    return withdrawalAmount;
  }

  function returnDeposit() public {
    uint withdrawalAmount = balance[msg.sender];
    balance[msg.sender] = 0;
    time[msg.sender] = 0;
    percentWithdraw[msg.sender] = 0;
    msg.sender.transfer(withdrawalAmount);
  }
}

код saveMath
// SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.9.0;

contract SafeMath {
  function mul(uint256 a, uint256 b) internal pure returns (uint256){
    if (a == 0){
      return 0;
    }

    uint256 c =a*b;
    assert(c / a == b);
    return c;
  }

  function div(uint256 a, uint256 b) internal pure returns (uint256){
    uint256 c = a/b;
    return c;
  }
  function sub(uint256 a, uint256 b) internal pure returns (uint256){
    assert(b <= a);
    return a - b;
  }
  function add(uint256 a, uint256 b) internal pure returns (uint256){
    uint256 c = a + b;
    assert(c>=a);
    return c;
  }
}

