'use strict';

/**
 * @ngdoc function
 * @name angularExApp.controller:AboutCtrl
 * @description
 * # AboutCtrl
 * Controller of the angularExApp
 */
angular.module('angularExApp')
  .controller('AboutCtrl', function ($scope) {
    $scope.awesomeThings = [
      'HTML5 Boilerplate',
      'AngularJS',
      'Karma'
    ];
  });

angular.module('angularExApp').controller('mycntroller', function ($scope) {});

