<form action="password_update.php" method="post">
    <fieldset>
        <div class="form-group">
            <input autofocus class="form-control" name="old_password" placeholder="Old Password" type="text"/>
        </div>
        <div class="form-group">
            <input class="form-control" name="new_password" placeholder="New Password" type="password"/>
        </div>
        <div class="form-group">
            <input class="form-control" name="confirm_password" placeholder="Confirm New Password" type="password"/>
        </div>
        <div class="form-group">
            <button type="submit" class="btn btn-default">Change Password</button>
        </div>
    </fieldset>
</form>
