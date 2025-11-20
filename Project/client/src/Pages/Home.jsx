import { useState } from 'react'
import { Navigate, useNavigate } from 'react-router-dom'
import {
  Box,
  Button,
  Container,
  Stack,
  Typography,
  Grid,
} from '@mui/material'
import { useGlobal } from '../SessionManager'
import BoardSection from '../Components/BoardSection'
import TaskboardDialog from '../Components/NewBoardDialogue'
import { addTaskboard } from '../APIManager'
import UserStats from '../Components/UserStats'
import LeaderboardSidebar from '../Components/LeaderboardSidebar'

function Home() {
  const { state, updateState } = useGlobal()
  const navigate = useNavigate()

  if (!state.loggedIn) {
    return <Navigate to="/login" replace />
  }

  const [isDialogOpen, setIsDialogOpen] = useState(false)

  const openDialog = () => setIsDialogOpen(true)
  const closeDialog = () => setIsDialogOpen(false)

  const handleSaveBoard = ({ name }) => {
    addTaskboard(name, state.user.userId).then(data => {
      if (data == null) return;

      let tmp = state.privateTaskboardList;
      tmp.push({
        taskboard_id: data.taskboard_id,
        name: name,
        tasks: [],
        users: [
          {
            userId: state.user.userId,
            username: state.user.username,
            isAdmin: true
          }
        ]
      })
      updateState({ privateTaskboardList: tmp })
    })
  }

  const handleBoardClick = (board) => {
    updateState({ currentTaskBoard: board })
    navigate('/board')
  }

  // Calculate user level based on XP (100 XP per level)
  const userLevel = Math.floor((state.user?.xp_points || 0) / 100) + 1

  return (
    <Box
      sx={{
        minHeight: '100vh',
        background: `linear-gradient(135deg, rgba(102, 126, 234, 0.1), rgba(118, 75, 162, 0.1)), url('/background.jpg')`,
        backgroundSize: 'cover',
        backgroundPosition: 'center',
        backgroundAttachment: 'fixed',
        py: 0,
        px: 2,
      }}
    >
      <Box sx={{ maxWidth: '1400px', mx: 'auto', pt: 3, px: 2 }}>
        <Grid container spacing={3} sx={{ display: 'flex', flexWrap: 'nowrap' }}>
          {/* Main Content - Takes up remaining space */}
          <Grid item sx={{ flex: 1, minWidth: 0 }}>
            <Stack spacing={3}>
              {/* Hero Section - No Box */}
              <Box>
                <Typography variant="h4" fontWeight={700} sx={{ mb: 1, fontSize: '2rem' }}>
                  Welcome back, {state.user?.username}!
                </Typography>
                <Typography variant="body1" color="text.secondary" sx={{ mb: 3 }}>
                  You're on Level {userLevel}. Keep building your productivity streak!
                </Typography>
                <Button
                  variant="contained"
                  size="large"
                  onClick={openDialog}
                  sx={{
                    background: 'linear-gradient(135deg, #c5bed6ff 0%, #b5b3cbff 100%)',
                    textTransform: 'none',
                    fontSize: '1rem',
                    fontWeight: 600,
                  }}
                >
                  Create New Board
                </Button>
              </Box>

              {/* Boards Section */}
              <BoardSection
                title="Personal Task Boards"
                boards={state.privateTaskboardList}
                onBoardClick={handleBoardClick}
              />
              <BoardSection
                title="Shared Task Boards"
                boards={state.sharedTaskboardList}
                onBoardClick={handleBoardClick}
              />
            </Stack>
          </Grid>

          {/* Leaderboard - Right Side Fixed Width */}
          <Grid item sx={{ width: '300px', flexShrink: 0 }}>
            <Box sx={{ position: 'sticky', top: '100px' }}>
              {/* User Stats Card at Top */}
              <UserStats user={state.user} userLevel={userLevel} />

              {/* Leaderboard */}
              <LeaderboardSidebar userId={state.user?.userId} />
            </Box>
          </Grid>
        </Grid>
      </Box>

      <TaskboardDialog
        open={isDialogOpen}
        title="New Task Board"
        onClose={closeDialog}
        onSave={handleSaveBoard}
      />
    </Box>
  )
}

export default Home