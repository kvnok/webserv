document.addEventListener('DOMContentLoaded', function () {
    const button = document.getElementById('clickMeButton');
    const message = document.getElementById('clickMessage');

    button.addEventListener('click', function () {
        message.style.display = 'block';
    });
});